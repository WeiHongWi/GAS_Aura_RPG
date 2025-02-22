// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/WidgetController/AuraWidgetController.h"

#include "Player/AuraPlayerController.h"
#include "Player/AuraPlayerState.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "AbilitySystem/AuraAttributeSet.h"
#include "AbilitySystem/Data/AbilityInfo.h"

void UAuraWidgetController::SetWidgetController(const FWidgetControllerParams& WCParams)
{
	PlayerController = WCParams.PlayerController;
	PlayerState = WCParams.PlayerState;
	AbilitySystemComp = WCParams.AbilitySystemComponent;
	AttributeSet = WCParams.AttributeSet;
}

void UAuraWidgetController::BroadcastInitValue()
{
}

void UAuraWidgetController::BindCallbacksToDependencies()
{
}

void UAuraWidgetController::BroadcastAbilityInfo()
{
	if (!GetAuraASC()->bIsStartupAbilityInitialized) return;

	FForEachAbility ForEachDelegate;
	ForEachDelegate.BindLambda(
		[this](const FGameplayAbilitySpec& AbilitySpec) {
			FAuraAbilityInfo info = AbilityInfo->GetAbilityInfoByTag(GetAuraASC()->GetAbilityTagBySpec(AbilitySpec));
			info.InputTag = GetAuraASC()->GetInputTagBySpec(AbilitySpec);
			info.StatusTag = GetAuraASC()->GetStatusTagBySpec(AbilitySpec);
			AbilityInfoDelegate.Broadcast(info);
		}
	);
	GetAuraASC()->ForEachAbility(ForEachDelegate);
}


AAuraPlayerController* UAuraWidgetController::GetAuraPC()
{
	if (AuraPlayerController == nullptr) {
		AuraPlayerController = Cast<AAuraPlayerController>(PlayerController);
	}

	return AuraPlayerController;
}

AAuraPlayerState* UAuraWidgetController::GetAuraPS()
{
	if (AuraPlayerState == nullptr) {
		AuraPlayerState = Cast<AAuraPlayerState>(PlayerState);
	}

	return AuraPlayerState;
}

UAuraAbilitySystemComponent* UAuraWidgetController::GetAuraASC()
{
	if (AuraAbilitySystemComp == nullptr) {
		AuraAbilitySystemComp = Cast<UAuraAbilitySystemComponent>(AbilitySystemComp);
	}

	return AuraAbilitySystemComp;
}

UAuraAttributeSet* UAuraWidgetController::GetAuraAS()
{
	if (AuraAttributeSet == nullptr) {
		AuraAttributeSet = Cast<UAuraAttributeSet>(AttributeSet);
	}

	return AuraAttributeSet;
}
