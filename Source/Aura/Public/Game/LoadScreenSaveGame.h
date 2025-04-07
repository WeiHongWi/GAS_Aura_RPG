// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "GameplayTagContainer.h"
#include "LoadScreenSaveGame.generated.h"

class UGameplayAbility;


UENUM(BlueprintType)
enum SaveSlotStatus
{
	Vacant,
	EnterName,
	Taken
};

USTRUCT()
struct FSavedActor {
	GENERATED_BODY()

	UPROPERTY()
	FName ActorName = FName();

	UPROPERTY()
	FTransform Transoform = FTransform();

	UPROPERTY()
	TArray<uint8> Bytes;
};
inline bool operator==(const FSavedActor Actor1, const FSavedActor Actor2) {
	return Actor1.ActorName == Actor2.ActorName;
}


USTRUCT()
struct FSavedMap {
	GENERATED_BODY()

	UPROPERTY()
	FString MapAssetName = FString();

	UPROPERTY()
	TArray<FSavedActor> SavedActors;
};

USTRUCT(BlueprintType)
struct FSaveGameAbility {
	GENERATED_BODY();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Class Defaults")
	TSubclassOf<UGameplayAbility> GameplayAbility;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	FGameplayTag AbilityTag = FGameplayTag();

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	FGameplayTag AbilityStatus = FGameplayTag();

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	FGameplayTag AbilitySlot = FGameplayTag();

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	FGameplayTag AbilityType = FGameplayTag();

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	int32 level;
};
inline bool operator==(const FSaveGameAbility& Left, const FSaveGameAbility& Right) {
	return Left.AbilityTag.MatchesTagExact(Right.AbilityTag);
}


/**
 * 
 */
UCLASS()
class AURA_API ULoadScreenSaveGame : public USaveGame
{
	GENERATED_BODY()
	
public:
	UPROPERTY()
	FString PlayerName = FString("Default Name");

	UPROPERTY()
	FString MapName = FString();

	UPROPERTY()
	int32 SlotIndex = 0;

	UPROPERTY()
	FString SlotName = FString();

	UPROPERTY()
	FName PlayerStartTag;

	UPROPERTY()
	TEnumAsByte<SaveSlotStatus> SlotStatus;

	/*Player State*/
	UPROPERTY()
	int32 level = 0;

	UPROPERTY()
	int32 exp = 0;

	UPROPERTY()
	int32 AttributePoints = 0;

	UPROPERTY()
	int32 SpellPoints = 0;

	UPROPERTY()
	float Strength = 0.f;

	UPROPERTY()
	float Intelligence = 0.f;

	UPROPERTY()
	float Resilience = 0.f;

	UPROPERTY()
	float Vigor = 0.f;

	UPROPERTY()
	bool bFirstLoadData = true;

	/* Save Game Ability*/
	UPROPERTY()
	TArray<FSaveGameAbility> SaveGameAbilities;

	UPROPERTY()
	TArray<FSavedMap> SavedMaps;

	FSavedMap GetSavedMapFromMapName(const FString& InMapName);
	bool HasMap(const FString& InMapName);
};
