// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/ViewModel/MVVM_LoadScreen.h"

#include "UI/ViewModel/MVVM_LoadSlot.h"
#include "Game/AuraGameModeBase.h"
#include "Game/LoadScreenSaveGame.h"
#include "Kismet/GameplayStatics.h"
#include "Game/AuraGameInstance.h"
#include "Player/AuraPlayerState.h"

void UMVVM_LoadScreen::InitializeSlot()
{
	LoadSlot_0 = NewObject<UMVVM_LoadSlot>(this,LoadSlotViewModelClass);
	LoadSlot_0->SetLocalSlotName(FString("LoadSlot_0"));
	LoadSlot_0->SlotIndex = 0;
	LoadSlotMap.Add(0, LoadSlot_0);

	LoadSlot_1 = NewObject<UMVVM_LoadSlot>(this, LoadSlotViewModelClass);
	LoadSlot_1->SetLocalSlotName(FString("LoadSlot_1"));
	LoadSlot_1->SlotIndex = 1;
	LoadSlotMap.Add(1, LoadSlot_1);

	LoadSlot_2 = NewObject<UMVVM_LoadSlot>(this, LoadSlotViewModelClass);
	LoadSlot_2->SetLocalSlotName(FString("LoadSlot_2"));
	LoadSlot_2->SlotIndex = 2;
	LoadSlotMap.Add(2, LoadSlot_2);

	SetNumOfSlot(LoadSlotMap.Num());
}

UMVVM_LoadSlot* UMVVM_LoadScreen::GetSlotByIndex(int32 index) const
{
	return LoadSlotMap.FindChecked(index);
}

void UMVVM_LoadScreen::NewGameButtonPressed(int32 index)
{
	LoadSlotMap[index]->SetWidgetSwitcherIndex.Broadcast(1);
}

void UMVVM_LoadScreen::NewSlotButtonPressed(int32 index,const FString& EnterName)
{
	AAuraGameModeBase* AuraGamemode = Cast<AAuraGameModeBase>(UGameplayStatics::GetGameMode(this));
	UAuraGameInstance* AuraGameInstance = Cast<UAuraGameInstance>(AuraGamemode->GetGameInstance());

	LoadSlotMap[index]->SetMapName(AuraGamemode->DefaultMapName);
	LoadSlotMap[index]->SetPlayerName(EnterName);
	LoadSlotMap[index]->SetLevel(1);
	LoadSlotMap[index]->SlotStatus = SaveSlotStatus::Taken;
	LoadSlotMap[index]->PlayerStartTag = AuraGamemode->PlayerStartTag;
	AuraGamemode->SaveSlotData(LoadSlotMap[index], index);
	LoadSlotMap[index]->InitializeSlot();

	AuraGameInstance->PlayerStartTag = AuraGamemode->PlayerStartTag;
	AuraGameInstance->LoadSlotIndex = LoadSlotMap[index]->SlotIndex;
	AuraGameInstance->LoadSlotName = LoadSlotMap[index]->GetLocalSlotName();
}

void UMVVM_LoadScreen::SelectSlotButtonPressed(int32 index)
{
	LoadScreenButtonEnable.Broadcast();
	for (const TTuple<int32, UMVVM_LoadSlot*> Pair : LoadSlotMap) {
		if (Pair.Key == index) {
			Pair.Value->SelectButtonEnable.Broadcast(false);
		}
		else {
			Pair.Value->SelectButtonEnable.Broadcast(true);
		}
	}

	SelectedSlot = LoadSlotMap[index];
}

void UMVVM_LoadScreen::LoadData()
{
	AAuraGameModeBase* AuraGamemode = Cast<AAuraGameModeBase>(UGameplayStatics::GetGameMode(this));
	
	for (const TTuple<int32, UMVVM_LoadSlot*>& Slot : LoadSlotMap) {
		ULoadScreenSaveGame* SaveGame = AuraGamemode->GetLoadSlot(Slot.Value->GetLocalSlotName(), Slot.Value->SlotIndex);
		
		const FString SlotName = SaveGame->PlayerName;
		TEnumAsByte<SaveSlotStatus> SlotStatus = SaveGame->SlotStatus;
		const int32 level = SaveGame->level;

		Slot.Value->SlotStatus = SlotStatus;
		Slot.Value->SetPlayerName(SlotName);
		Slot.Value->SetMapName(SaveGame->MapName);
		Slot.Value->SetLevel(level);
		Slot.Value->PlayerStartTag = SaveGame->PlayerStartTag;
		Slot.Value->InitializeSlot();
	}
}

void UMVVM_LoadScreen::DeleteButtonPressed()
{
	if (IsValid(SelectedSlot)) {
		AAuraGameModeBase::DeleteSlotData(SelectedSlot->GetLocalSlotName(), SelectedSlot->SlotIndex);
		SelectedSlot->SlotStatus = Vacant;
		SelectedSlot->InitializeSlot();
		SelectedSlot->SelectButtonEnable.Broadcast(true);
	}
}

void UMVVM_LoadScreen::PlayButtonPressed()
{
	AAuraGameModeBase* AuraGamemode = Cast<AAuraGameModeBase>(UGameplayStatics::GetGameMode(this));
	UAuraGameInstance* AuraGameInstance = Cast<UAuraGameInstance>(AuraGamemode->GetGameInstance());
	AuraGameInstance->PlayerStartTag = SelectedSlot->PlayerStartTag;
	AuraGameInstance->LoadSlotIndex = SelectedSlot->SlotIndex;
	AuraGameInstance->LoadSlotName = SelectedSlot->GetLocalSlotName();

	if (IsValid(SelectedSlot)) {
		AuraGamemode->TravelToMap(SelectedSlot);
	}
}

void UMVVM_LoadScreen::SetNumOfSlot(int32 number)
{
	UE_MVVM_SET_PROPERTY_VALUE(NumOfSlot, number);
}
