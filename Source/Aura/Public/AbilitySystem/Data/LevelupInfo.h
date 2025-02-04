// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "LevelupInfo.generated.h"

/**
 * 
 */
USTRUCT(BlueprintType)
struct FAuraLevelupInfo
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly,EditDefaultsOnly)
	int32 RequiredExperience = 0;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	int32 SpellPointReward = 1;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	int32 AttributePointReward = 1;
};

UCLASS()
class AURA_API ULevelupInfo : public UDataAsset
{
	GENERATED_BODY()
	
public:
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "LevelupInfo")
	TArray<FAuraLevelupInfo> LevelupInfo;

	UFUNCTION()
	int32 FindLevelFromAmountOfExp(int32 Exp);
};
