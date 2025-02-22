// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "AuraGameplayAbility.generated.h"

/**
 * 
 */
UCLASS()
class AURA_API UAuraGameplayAbility : public UGameplayAbility
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditDefaultsOnly, Category = "Ability")
	FGameplayTag StartupInputTag;

	virtual FString GetCurrentDescription(int32 level);

	virtual FString GetNextDescription(int32 level);

	static FString GetLockedSpellDescription(int32 level);

	float GetManaCost(float InLevel = 1.f);

	float GetCooldown(float InLevel = 1.f);
};
