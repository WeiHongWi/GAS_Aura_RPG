// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/AuraDamageGameplayAbility.h"
#include "AuraArcaneShard.generated.h"

/**
 * 
 */
UCLASS()
class AURA_API UAuraArcaneShard : public UAuraDamageGameplayAbility
{
	GENERATED_BODY()
	
public:
	virtual FString GetCurrentDescription(int32 level) override;
	virtual FString GetNextDescription(int32 level) override;
};
