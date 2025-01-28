// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/AuraGameplayAbility.h"
#include "AuraSummonAbility.generated.h"

/**
 * 
 */
UCLASS()
class AURA_API UAuraSummonAbility : public UAuraGameplayAbility
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable)
	TArray<FVector> GetSpawnLocation();

	UFUNCTION(BlueprintCallable)
	TSubclassOf<APawn> GetRandomMinion();

	UPROPERTY(EditAnywhere)
	TArray<TSubclassOf<APawn>> MinionClass;

	UPROPERTY(EditAnywhere)
	int NumMinions = 5;

	UPROPERTY(EditAnywhere)
	float MaxSpawnDistance = 100.f;

	UPROPERTY(EditAnywhere)
	float MinSpawnDistance = 60.f;

	UPROPERTY(EditAnywhere)
	float Spread = 90.f;
};
