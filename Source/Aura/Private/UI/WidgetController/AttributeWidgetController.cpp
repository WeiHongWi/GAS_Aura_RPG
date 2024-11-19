// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/WidgetController/AttributeWidgetController.h"

#include "AbilitySystem/AuraAttributeSet.h"
#include "AbilitySystem/Data/AttributeInfo.h"
#include "AuraGameplayTags.h"

void UAttributeWidgetController::BroadcastInitValue()
{
	UAuraAttributeSet* AuraAS = Cast<UAuraAttributeSet>(AttributeSet);

	check(AttributeInfo);


	for (auto& Pair : AuraAS->TagsToAttribute) {
		FAuraAttributesInfo Info = AttributeInfo->FindAttributeForTag(Pair.Key,true);
		Info.AttributeValue = Pair.Value().GetNumericValue(AuraAS);
		AttributeInfoDelegate.Broadcast(Info);
	}
}

void UAttributeWidgetController::BindCallbacksToDependencies()
{
	UAuraAttributeSet* AuraAS = Cast<UAuraAttributeSet>(AttributeSet);

	for (auto& Pair : AuraAS->TagsToAttribute) {
		AbilitySystemComp->GetGameplayAttributeValueChangeDelegate(Pair.Value()).AddLambda(
			[this,Pair,AuraAS](const FOnAttributeChangeData& Data) {
				FAuraAttributesInfo Info = AttributeInfo->FindAttributeForTag(Pair.Key, true);
				Info.AttributeValue = Pair.Value().GetNumericValue(AuraAS);
				AttributeInfoDelegate.Broadcast(Info);
			}
		);
	}

}
