// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "AbilitySystemComponent.h"
#include "AttributeSet.h"
#include "AuraWidgetController.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(
	FOnPlayerStateChangeSignature,
	int32, value
);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(
	FAbilityInfoSignature
	, const FAuraAbilityInfo&,
	Info
);

class UAbilitySystemComponent;
class UAttributeSet;
class AAuraPlayerController;
class AAuraPlayerState;
class UAuraAbilitySystemComponent;
class UAuraAttributeSet;
class UAbilityInfo;

USTRUCT(BlueprintType)
struct FWidgetControllerParams {
	GENERATED_BODY()

	FWidgetControllerParams() {}

	FWidgetControllerParams(APlayerController* AC, APlayerState* APS, UAbilitySystemComponent* ASC, UAttributeSet* AS)
		:PlayerController(AC), PlayerState(APS), AbilitySystemComponent(ASC), AttributeSet(AS) {}
		
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	TObjectPtr<APlayerController> PlayerController = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr < APlayerState> PlayerState = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr < UAbilitySystemComponent> AbilitySystemComponent = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr < UAttributeSet> AttributeSet = nullptr;
};
/**
 * 
 */
UCLASS()
class AURA_API UAuraWidgetController : public UObject
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable)
	void SetWidgetController(const struct FWidgetControllerParams& WCParams);

	
	virtual void BroadcastInitValue();
	virtual void BindCallbacksToDependencies();

	UPROPERTY(BlueprintAssignable, Category = "GAS|Messages")
	FAbilityInfoSignature AbilityInfoDelegate;

	void BroadcastAbilityInfo();
protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ability Info")
	TObjectPtr<UAbilityInfo> AbilityInfo;

	UPROPERTY(BlueprintReadOnly, Category = "WidgetController")
	TObjectPtr<APlayerController> PlayerController;

	UPROPERTY(BlueprintReadOnly, Category = "WidgetController")
	TObjectPtr<APlayerState> PlayerState;

	UPROPERTY(BlueprintReadOnly, Category = "WidgetController")
	TObjectPtr<UAbilitySystemComponent> AbilitySystemComp;

	UPROPERTY(BlueprintReadOnly, Category = "WidgetController")
	TObjectPtr<UAttributeSet> AttributeSet;

	UPROPERTY(BlueprintReadOnly, Category = "WidgetController")
	TObjectPtr<AAuraPlayerController> AuraPlayerController;

	UPROPERTY(BlueprintReadOnly, Category = "WidgetController")
	TObjectPtr<AAuraPlayerState> AuraPlayerState;

	UPROPERTY(BlueprintReadOnly, Category = "WidgetController")
	TObjectPtr<UAuraAbilitySystemComponent> AuraAbilitySystemComp;

	UPROPERTY(BlueprintReadOnly, Category = "WidgetController")
	TObjectPtr<UAuraAttributeSet> AuraAttributeSet;

	AAuraPlayerController* GetAuraPC();
	AAuraPlayerState* GetAuraPS();
	UAuraAbilitySystemComponent* GetAuraASC();
	UAuraAttributeSet* GetAuraAS();
};
