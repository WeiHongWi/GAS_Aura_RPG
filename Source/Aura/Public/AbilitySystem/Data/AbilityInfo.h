// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "GameplayTagContainer.h"
#include "AbilityInfo.generated.h"

USTRUCT(BlueprintType)
struct FAuraAbilityInfo
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly,EditDefaultsOnly)
	FGameplayTag AbilityTag = FGameplayTag();

	UPROPERTY(BlueprintReadOnly)
	FGameplayTag InputTag = FGameplayTag();

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	FGameplayTag CooldownTag = FGameplayTag();

	//Background : Material Instance
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	TObjectPtr<const UMaterialInstance> Background = nullptr;
	//Icon : Image
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	TObjectPtr<const UTexture2D> Icon = nullptr;
};

/**
 * 
 */
UCLASS()
class AURA_API UAbilityInfo : public UDataAsset
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "AbilityInfo")
	TArray<FAuraAbilityInfo> AbilityInformation;

	FAuraAbilityInfo GetAbilityInfoByTag(const FGameplayTag& tag, bool bLog=false) const;
};
