// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/AuraGameModeBase.h"

#include "Game/LoadScreenSaveGame.h"
#include "Kismet/GameplayStatics.h"
#include "UI/ViewModel/MVVM_LoadSlot.h"
#include "GameFramework/PlayerStart.h"
#include "Game/AuraGameInstance.h"
#include "EngineUtils.h"
#include "Interaction/SaveInterface.h"
#include "Serialization/ObjectAndNameAsStringProxyArchive.h"

ULoadScreenSaveGame* AAuraGameModeBase::GetLoadSlot(const FString SlotName, const int32 SlotIndex) const
{
	USaveGame* LoadGame = nullptr;
	if (UGameplayStatics::DoesSaveGameExist(SlotName, SlotIndex)) {
		LoadGame = UGameplayStatics::LoadGameFromSlot(SlotName, SlotIndex);
	}else{
		LoadGame = UGameplayStatics::CreateSaveGameObject(LoadScreenSaveGameClass);
	}

	ULoadScreenSaveGame* LoadScreenSaveGame = Cast<ULoadScreenSaveGame>(LoadGame);
	
	return LoadScreenSaveGame;
}

ULoadScreenSaveGame* AAuraGameModeBase::RetrieveInGameSaveData() const
{
	UAuraGameInstance* AuraGameInstance = Cast<UAuraGameInstance>(GetGameInstance());
	ULoadScreenSaveGame* LoadScreenSaveGame = nullptr;
	if (AuraGameInstance) {
		const FString LoadSlotName = AuraGameInstance->LoadSlotName;
		const int32 LoadSlotIndex = AuraGameInstance->LoadSlotIndex;

		LoadScreenSaveGame = GetLoadSlot(LoadSlotName, LoadSlotIndex);
	}

	return LoadScreenSaveGame;
}

void AAuraGameModeBase::TravelToMap(UMVVM_LoadSlot* LoadSlot)
{
	const FString LocalSlotName = LoadSlot->GetLocalSlotName();
	const int32 Index = LoadSlot->SlotIndex;

	UGameplayStatics::OpenLevelBySoftObjectPtr(LoadSlot, Maps.FindChecked(LoadSlot->GetMapName()));
}

void AAuraGameModeBase::SaveSlotData(UMVVM_LoadSlot* LoadSlot, int32 SlotIndex)
{
	if (UGameplayStatics::DoesSaveGameExist(LoadSlot->GetLocalSlotName(),SlotIndex)) {
		UGameplayStatics::DeleteGameInSlot(LoadSlot->GetLocalSlotName(), SlotIndex);
	}
	USaveGame* SaveGame = UGameplayStatics::CreateSaveGameObject(LoadScreenSaveGameClass);
	ULoadScreenSaveGame* LoadScreenSaveGame = Cast<ULoadScreenSaveGame>(SaveGame);

	LoadScreenSaveGame->MapName = LoadSlot->GetMapName();
	LoadScreenSaveGame->PlayerName = LoadSlot->GetPlayerName();
	LoadScreenSaveGame->SlotStatus = Taken;
	LoadScreenSaveGame->PlayerStartTag = LoadSlot->PlayerStartTag;

	UGameplayStatics::SaveGameToSlot(LoadScreenSaveGame, LoadSlot->GetLocalSlotName(), SlotIndex);

}

void AAuraGameModeBase::SaveInProgressData(ULoadScreenSaveGame* SaveGame)
{
	UAuraGameInstance* AuraGameInstance = Cast<UAuraGameInstance>(GetGameInstance());

	const FString LoadSlotName = AuraGameInstance->LoadSlotName;
	const int32 LoadSlotIndex = AuraGameInstance->LoadSlotIndex;
	AuraGameInstance->PlayerStartTag = SaveGame->PlayerStartTag;

	UGameplayStatics::SaveGameToSlot(SaveGame, LoadSlotName, LoadSlotIndex);
}

void AAuraGameModeBase::SaveWorldState(UWorld* World)
{
	FString WorldName = World->GetMapName();
	WorldName.RemoveFromStart(World->StreamingLevelsPrefix);

	UAuraGameInstance* AuraGI = Cast<UAuraGameInstance>(GetGameInstance());
	check(AuraGI);

	if (ULoadScreenSaveGame* SaveGame = GetLoadSlot(AuraGI->LoadSlotName,AuraGI->LoadSlotIndex)) {
		if (!SaveGame->HasMap(WorldName)) {
			FSavedMap NewSaveMap;
			NewSaveMap.MapAssetName = WorldName;
			SaveGame->SavedMaps.Add(NewSaveMap);
		}
		
		FSavedMap SaveMap = SaveGame->GetSavedMapFromMapName(WorldName);
		SaveMap.SavedActors.Empty();
		for (FActorIterator It(World); It; ++It) {
			AActor* actor = *It;

			if (!IsValid(actor) || !actor->Implements<USaveInterface>()) continue;

			FSavedActor SavedActor;
			SavedActor.ActorName = actor->GetFName();
			SavedActor.Transoform = actor->GetTransform();

			FMemoryWriter MemoryWriter(SavedActor.Bytes);
			FObjectAndNameAsStringProxyArchive Archive(MemoryWriter,true);
			Archive.ArIsSaveGame = true;

			actor->Serialize(Archive);

			SaveMap.SavedActors.AddUnique(SavedActor);
		}

		for (FSavedMap& MapToReplace : SaveGame->SavedMaps) {
			if (MapToReplace.MapAssetName == WorldName) {
				MapToReplace = SaveMap;
			}
		}

		UGameplayStatics::SaveGameToSlot(SaveGame, AuraGI->LoadSlotName, AuraGI->LoadSlotIndex);
	}
}

void AAuraGameModeBase::LoadWorldState(UWorld* World)
{
	FString WorldName = World->GetMapName();
	WorldName.RemoveFromStart(World->StreamingLevelsPrefix);

	UAuraGameInstance* AuraGI = Cast<UAuraGameInstance>(GetGameInstance());
	check(AuraGI);

	if (UGameplayStatics::DoesSaveGameExist(AuraGI->LoadSlotName, AuraGI->LoadSlotIndex)) {
		ULoadScreenSaveGame* SaveGame = Cast<ULoadScreenSaveGame>(UGameplayStatics::LoadGameFromSlot(AuraGI->LoadSlotName, AuraGI->LoadSlotIndex));
		
		if (SaveGame == nullptr) {
			UE_LOG(LogTemp,Warning,TEXT("I am TRASH"));
		}
		for (FActorIterator it(World); it; ++it) {
			AActor* Actor = *it;
			if (!Actor->Implements<USaveInterface>()) continue;

			for (FSavedActor SaveActor:SaveGame->GetSavedMapFromMapName(WorldName).SavedActors) {
				if (SaveActor.ActorName == Actor->GetFName()) {
					if (ISaveInterface::Execute_ShouldLoadTransform(Actor)) {
						Actor->SetActorTransform(SaveActor.Transoform);
					}

					FMemoryReader MemoryReader(SaveActor.Bytes);

					FObjectAndNameAsStringProxyArchive Archive(MemoryReader, true);
					Archive.ArIsSaveGame = true;
					Actor->Serialize(Archive);

					ISaveInterface::Execute_LoadActor(Actor);
				}
			}
		}
	}
}

void AAuraGameModeBase::DeleteSlotData(const FString LoadSlotName, int32 SlotIndex)
{
	if (UGameplayStatics::DoesSaveGameExist(LoadSlotName, SlotIndex)) {
		UGameplayStatics::DeleteGameInSlot(LoadSlotName, SlotIndex);
	}
}

AActor* AAuraGameModeBase::ChoosePlayerStart_Implementation(AController* Player)
{
	UAuraGameInstance* AuraGameInstance = Cast<UAuraGameInstance>(GetGameInstance());
	TArray<AActor*> Actors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(),APlayerStart::StaticClass(), Actors);
	if (Actors.Num() <= 0) return nullptr;

	AActor* SelectedActor = Actors[0];
	for (AActor* actor : Actors) {
		if (APlayerStart* PlayerStart = Cast<APlayerStart>(actor)) {
			if (PlayerStart->PlayerStartTag == AuraGameInstance->PlayerStartTag) {
				SelectedActor = actor;
				return SelectedActor;
			}
		}
	}

	return nullptr;
}

void AAuraGameModeBase::BeginPlay()
{
	Super::BeginPlay();
	Maps.Add(DefaultMapName, DefaultMap);
}
