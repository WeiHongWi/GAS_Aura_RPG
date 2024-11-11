// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/WidgetController/AuraWidgetController.h"
#include "OverlayAuraWidgetController.generated.h"

/**
 * 
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(
	FOnHealthChangeSignature,
	float, Health
);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(
	FOnMaxHealthChangeSignature,
	float, MaxHealth
);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(
	FOnManaChangeSignature,
	float, Mana
);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(
	FOnMaxManaChangeSignature,
	float, MaxMana
);

UCLASS(BlueprintType,Blueprintable)
class AURA_API UOverlayAuraWidgetController : public UAuraWidgetController
{
	GENERATED_BODY()
	
public:
	virtual void BroadcastInitValue() override;
	virtual void BindCallbacksToDependencies() override;

	UPROPERTY(BlueprintAssignable, Category = "GAS|Attribute")
	FOnHealthChangeSignature OnHealthChange;

	UPROPERTY(BlueprintAssignable, Category = "GAS|Attribute")
	FOnMaxHealthChangeSignature OnMaxHealthChange;

	UPROPERTY(BlueprintAssignable, Category = "GAS|Attribute")
	FOnManaChangeSignature OnManaChange;

	UPROPERTY(BlueprintAssignable, Category = "GAS|Attribute")
	FOnMaxManaChangeSignature OnMaxManaChange;

protected:
	void HealthChange(const FOnAttributeChangeData& data) const;

	void MaxHealthChange(const FOnAttributeChangeData& data) const;

	void ManaChange(const FOnAttributeChangeData& data) const;

	void MaxManaChange(const FOnAttributeChangeData& data) const;
};
