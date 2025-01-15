// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "Interaction/EnemyInterface.h"
#include "GameplayTagContainer.h"
#include "AuraPlayerController.generated.h"


class UInputMappingContext;
class UInputAction;
class IEnemyInterface;
class UInputActionDataAsset;
class UAuraAbilitySystemComponent;
class USplineComponent;
class UDamageWidgetComponent;
/**
 * 
 */
UCLASS()
class AURA_API AAuraPlayerController : public APlayerController
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<UInputMappingContext> AuraContext;

	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<UInputAction> MoveAction;

	UPROPERTY(EditAnywhere,Category="Input")
	TObjectPtr<UInputAction> ShiftAction;

	void ShiftPress();
	void ShiftRelease();
	bool bIsShiftPress = false;

	TScriptInterface<IEnemyInterface> LastActor;
	TScriptInterface<IEnemyInterface> ThisActor;
	FHitResult CursorHit;

	void Move(const struct FInputActionValue& ActionValue);

	void CursorTrace();
public:
	AAuraPlayerController();

	virtual void PlayerTick(float DeltaTime) override;

	UFUNCTION(Client, Reliable)
	void ShowDamageNumber(float DamageAmount, ACharacter* TargetCharacter, bool bBlockedHit, bool bCriticalHit);

protected:
	virtual void BeginPlay() override;

	virtual void SetupInputComponent() override;
private:
	void AbilityTagPress(FGameplayTag GameplayTag);
	void AbilityTagRelease(FGameplayTag GameplayTag);
	void AbilityTagHeld(FGameplayTag GameplayTag);

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UInputActionDataAsset> InputConfig;

	UPROPERTY()
	TObjectPtr<UAuraAbilitySystemComponent> AuraAbilitySystemComp;

	UAuraAbilitySystemComponent* GetASC(); 

	FVector CachedDestination = FVector::ZeroVector;
	float FollowTime = 0.f;
	float ShortPressThreshold = 0.5f;
	bool bAutoRunning = false;
	bool bIsTargeting = false;

	UPROPERTY(EditDefaultsOnly)
	float AutoRunningAcceptRadius = 50.f;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USplineComponent> Spline;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UDamageWidgetComponent> DamageWidgetCompClass;

	void AutoRun();
};
