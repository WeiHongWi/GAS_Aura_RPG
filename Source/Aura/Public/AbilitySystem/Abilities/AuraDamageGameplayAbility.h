// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/AuraGameplayAbility.h"
#include "AuraDamageGameplayAbility.generated.h"

/**
 * 
 */
UCLASS()
class AURA_API UAuraDamageGameplayAbility : public UAuraGameplayAbility
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	void CauseDamage(AActor* Target);
protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<UGameplayEffect> DamageEffectClass;


	UPROPERTY(EditDefaultsOnly, Category = "Damage")
	TMap<FGameplayTag, FScalableFloat> DamageTypes;

	float GetDamageByDamageType(float InLevel = 1.f,const FGameplayTag DamageType = FGameplayTag());
};
