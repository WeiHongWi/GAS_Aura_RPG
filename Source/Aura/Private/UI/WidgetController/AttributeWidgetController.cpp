// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/WidgetController/AttributeWidgetController.h"

#include "AbilitySystem/AuraAttributeSet.h"
#include "AbilitySystem/Data/AttributeInfo.h"
#include "AuraGameplayTags.h"
#include "Player/AuraPlayerState.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"

void UAttributeWidgetController::BroadcastInitValue()
{
	check(AttributeInfo);

	for (auto& Pair : GetAuraAS()->TagsToAttribute) {
		FAuraAttributesInfo Info = AttributeInfo->FindAttributeForTag(Pair.Key,true);
		Info.AttributeValue = Pair.Value().GetNumericValue(GetAuraAS());
		AttributeInfoDelegate.Broadcast(Info);
	}

	PointsChangeDelegate.Broadcast(GetAuraPS()->GetAttributePoints());
}

void UAttributeWidgetController::BindCallbacksToDependencies()
{
	for (auto& Pair : GetAuraAS()->TagsToAttribute) {
		AbilitySystemComp->GetGameplayAttributeValueChangeDelegate(Pair.Value()).AddLambda(
			[this,Pair](const FOnAttributeChangeData& Data) {
				FAuraAttributesInfo Info = AttributeInfo->FindAttributeForTag(Pair.Key, true);
				Info.AttributeValue = Pair.Value().GetNumericValue(GetAuraAS());
				AttributeInfoDelegate.Broadcast(Info);
			}
		);
	}

	GetAuraPS()->AttributePointsChange.AddLambda([this](int32 points) {
		PointsChangeDelegate.Broadcast(points);
		});
}

void UAttributeWidgetController::UpgradeAttributePoints(const FGameplayTag& AttributeTag)
{
	UAuraAbilitySystemComponent* AuraASC = Cast<UAuraAbilitySystemComponent>(AbilitySystemComp);
	check(AuraASC);

	AuraASC->UpgradeAttributeByPoint(AttributeTag);
}
