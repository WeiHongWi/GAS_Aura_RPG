// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "GameplayTagContainer.h"
#include "CombatInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI,BlueprintType)
class UCombatInterface : public UInterface
{
	GENERATED_BODY()
};


USTRUCT(BlueprintType)
struct FTagMontage {
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly)
	UAnimMontage* Montage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FGameplayTag WeaponTag;
};
/**
 * 
 */
class AURA_API ICombatInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	virtual int32 GetPlayerLevel();

	virtual FVector GetPartSocketLocation();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	TArray<FTagMontage> GetAttackMontages();

	UFUNCTION(BlueprintNativeEvent,BlueprintCallable)
	FVector GetWeaponTipSocketLocation(const FGameplayTag& MontageTag);

	UFUNCTION(BlueprintImplementableEvent,BlueprintCallable)
	void UpdateFacingTarget(const FVector& Target);

	UFUNCTION(BlueprintNativeEvent,BlueprintCallable)
	UAnimMontage* GetHitReactAnimMontage();

	UFUNCTION(BlueprintCallable,BlueprintNativeEvent)
	AActor* GetCombatTarget();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void SetCombatTarget(AActor* CombatTarget);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	bool bIsDead() const;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	AActor* GetAvatarActor();

	virtual void Die()  = 0;

};
