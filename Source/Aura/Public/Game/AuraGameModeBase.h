// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "AuraGameModeBase.generated.h"

class USaveGame;
class UMVVM_LoadSlot;
class UCharacterClassInfo;
class UAbilityInfo;
class ULoadScreenSaveGame;
class UEnemyDrops;

/**
 * 
 */
UCLASS()
class AURA_API AAuraGameModeBase : public AGameModeBase
{
	GENERATED_BODY()

public:
	ULoadScreenSaveGame* GetLoadSlot(const FString SlotName, const int32 SlotIndex) const;

	ULoadScreenSaveGame* RetrieveInGameSaveData() const;
	
	void TravelToMap(UMVVM_LoadSlot* LoadSlot);

	void SaveSlotData(UMVVM_LoadSlot* LoadSlot, int32 SlotIndex);

	void SaveInProgressData(ULoadScreenSaveGame* SaveGame);

	void SaveWorldState(UWorld* World);

	void LoadWorldState(UWorld* World);

	static void DeleteSlotData(const FString LoadSlotName, int32 SlotIndex);


	UPROPERTY(EditDefaultsOnly, Category = "Character Class Defaults")
	TObjectPtr<UCharacterClassInfo> CharacterClassInfo;

	UPROPERTY(EditDefaultsOnly, Category = "Ability Info")
	TObjectPtr<UAbilityInfo> AuraAbilityInfo;

	UPROPERTY(EditDefaultsOnly, Category = "Enemy Drops")
	TObjectPtr<UEnemyDrops> EnemyDrops;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<USaveGame> LoadScreenSaveGameClass;

	UPROPERTY(EditDefaultsOnly)
	FString DefaultMapName;

	UPROPERTY(EditDefaultsOnly)
	TSoftObjectPtr<UWorld> DefaultMap;

	UPROPERTY(EditDefaultsOnly)
	TMap<FString, TSoftObjectPtr<UWorld>> Maps;

	UPROPERTY(EditDefaultsOnly)
	FName PlayerStartTag;

	virtual AActor* ChoosePlayerStart_Implementation(AController* Player) override;
protected:
	virtual void BeginPlay() override;
};
