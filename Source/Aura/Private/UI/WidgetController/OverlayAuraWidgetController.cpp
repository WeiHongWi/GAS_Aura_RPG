// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/WidgetController/OverlayAuraWidgetController.h"
#include "AbilitySystem/AuraAttributeSet.h"

void UOverlayAuraWidgetController::BroadcastInitValue()
{
	UAuraAttributeSet* AuraAS = CastChecked<UAuraAttributeSet>(AttributeSet);

	OnHealthChange.Broadcast(AuraAS->GetHealth());
	OnMaxHealthChange.Broadcast(AuraAS->GetMaxHealth());
	OnManaChange.Broadcast(AuraAS->GetMana());
	OnMaxManaChange.Broadcast(AuraAS->GetMaxMana());
}

void UOverlayAuraWidgetController::BindCallbacksToDependencies()
{
	const UAuraAttributeSet* AuraAS = CastChecked<UAuraAttributeSet>(AttributeSet);
	

	AbilitySystemComp->GetGameplayAttributeValueChangeDelegate(AuraAS->GetHealthAttribute())
		.AddUObject(this, &UOverlayAuraWidgetController::HealthChange);

	AbilitySystemComp->GetGameplayAttributeValueChangeDelegate(AuraAS->GetMaxHealthAttribute())
		.AddUObject(this, &UOverlayAuraWidgetController::MaxHealthChange);

	AbilitySystemComp->GetGameplayAttributeValueChangeDelegate(AuraAS->GetManaAttribute())
		.AddUObject(this, &UOverlayAuraWidgetController::ManaChange);

	AbilitySystemComp->GetGameplayAttributeValueChangeDelegate(AuraAS->GetMaxManaAttribute())
		.AddUObject(this, &UOverlayAuraWidgetController::MaxManaChange);
}

void UOverlayAuraWidgetController::HealthChange(const FOnAttributeChangeData& data) const
{
	OnHealthChange.Broadcast(data.NewValue);
}

void UOverlayAuraWidgetController::MaxHealthChange(const FOnAttributeChangeData& data) const
{
	OnMaxHealthChange.Broadcast(data.NewValue);
}

void UOverlayAuraWidgetController::ManaChange(const FOnAttributeChangeData& data) const
{
	OnManaChange.Broadcast(data.NewValue);
}

void UOverlayAuraWidgetController::MaxManaChange(const FOnAttributeChangeData& data) const
{
	OnMaxManaChange.Broadcast(data.NewValue);
}


