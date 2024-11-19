// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/WidgetController/OverlayAuraWidgetController.h"
#include "AbilitySystem/AuraAttributeSet.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"

void UOverlayAuraWidgetController::BroadcastInitValue()
{
	UAuraAttributeSet* AuraAS = CastChecked<UAuraAttributeSet>(AttributeSet);

	OnHealthChange.Broadcast(AuraAS->GetHealth());
	OnManaChange.Broadcast(AuraAS->GetMana());
	OnMaxHealthChange.Broadcast(AuraAS->GetMaxHealth());
	OnMaxManaChange.Broadcast(AuraAS->GetMaxMana());
}

void UOverlayAuraWidgetController::BindCallbacksToDependencies()
{
	const UAuraAttributeSet* AuraAS = CastChecked<UAuraAttributeSet>(AttributeSet);
	

	AbilitySystemComp->GetGameplayAttributeValueChangeDelegate(AuraAS->GetHealthAttribute())
		.AddLambda(
			[this](const FOnAttributeChangeData& data) {
				OnHealthChange.Broadcast(data.NewValue);
			}
		);

	AbilitySystemComp->GetGameplayAttributeValueChangeDelegate(AuraAS->GetManaAttribute())
		.AddLambda(
			[this](const FOnAttributeChangeData& data) {
				OnManaChange.Broadcast(data.NewValue);
			});

	AbilitySystemComp->GetGameplayAttributeValueChangeDelegate(AuraAS->GetMaxHealthAttribute())
		.AddLambda(
			[this](const FOnAttributeChangeData& data) {
				OnMaxHealthChange.Broadcast(data.NewValue);
			}
		);

	AbilitySystemComp->GetGameplayAttributeValueChangeDelegate(AuraAS->GetMaxManaAttribute())
		.AddLambda(
			[this](const FOnAttributeChangeData& data) {
				OnMaxManaChange.Broadcast(data.NewValue);
			});

	Cast<UAuraAbilitySystemComponent>(AbilitySystemComp)->EffectTags.AddLambda(
		[this](const FGameplayTagContainer& EffectTag){
			for (const FGameplayTag& Tag : EffectTag) {
				FGameplayTag Req_Tag = FGameplayTag::RequestGameplayTag(FName("Message"));
				if (Tag.MatchesTag(Req_Tag)) {
					const FUIWidgetRow* Row = GetDatatableRowByTag<FUIWidgetRow>(WidgetDT, Tag);
					MessageWidgetDelegate.Broadcast(*Row);
				}
			}
		}

	);
}




