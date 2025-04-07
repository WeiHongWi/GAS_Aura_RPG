// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/AuraDamageGameplayAbility.h"
#include "AuraBeamSpell.generated.h"

/**
 * 
 */
UCLASS()
class AURA_API UAuraBeamSpell : public UAuraDamageGameplayAbility
{
	GENERATED_BODY()
	
public:
	virtual FString GetCurrentDescription(int32 level) override;
	virtual FString GetNextDescription(int32 level) override;
protected:
	UPROPERTY(BlueprintReadWrite, Category = "Beam")
	TObjectPtr < AActor> CursorHitActor;

	UPROPERTY(BlueprintReadWrite, Category = "Beam")
	FVector CursorHitLocation;

	UPROPERTY(BlueprintReadWrite, Category = "Beam")
	TObjectPtr<APlayerController> OwnerPlayerController;

	UPROPERTY(BlueprintReadWrite, Category = "Beam")
	TObjectPtr<ACharacter> OwnerCharacter;

	int32 NumOfTargets = 1;
public:
	UFUNCTION(BlueprintCallable)
	void StoreOwnerVariables();

	UFUNCTION(BlueprintCallable)
	void StoreCursorHitTarget(const FHitResult& HitResult);

	UFUNCTION(BlueprintCallable)
	void TraceFirstTarget(const FVector TargetLocation);

	UFUNCTION(BlueprintCallable)
	void StoreAdditionalTarget(TArray<AActor*>& OtherActors);

	UFUNCTION(BlueprintImplementableEvent)
	void PrimaryTargetDeath(AActor* FirstActor);

	UFUNCTION(BlueprintImplementableEvent)
	void AdditionalTargetDeath(AActor* AdditionalTarget);
};
