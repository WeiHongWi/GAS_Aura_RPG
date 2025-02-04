// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/AsyncTask/WaitCooldownChange.h"
#include "AbilitySystemComponent.h"

UWaitCooldownChange* UWaitCooldownChange::WaitForCooldownChange(UAbilitySystemComponent* ASC, const FGameplayTag& IncomeTag)
{
	UWaitCooldownChange* WaitCooldownChange = NewObject<UWaitCooldownChange>();
	WaitCooldownChange->ASC = ASC;
	WaitCooldownChange->CooldownTag = IncomeTag;
	if (!IsValid(ASC) || !IncomeTag.IsValid()) {
		WaitCooldownChange->EndTask();
		return nullptr;
	}
	ASC->RegisterGameplayTagEvent(
		IncomeTag,
		EGameplayTagEventType::NewOrRemoved)
		.AddUObject(WaitCooldownChange, &UWaitCooldownChange::CooldownTagChange);

	ASC->OnActiveGameplayEffectAddedDelegateToSelf.AddUObject(WaitCooldownChange,&UWaitCooldownChange::OnActiveEffectAdded);
	
	return WaitCooldownChange;
}

void UWaitCooldownChange::EndTask()
{
	if (!IsValid(this->ASC)) return;

	this->ASC->RegisterGameplayTagEvent(CooldownTag, EGameplayTagEventType::NewOrRemoved).RemoveAll(this);
	SetReadyToDestroy();
	MarkAsGarbage();
}

void UWaitCooldownChange::CooldownTagChange(const FGameplayTag Tag, int32 NewCount)
{
	if (NewCount == 0) {
		CooldownEnd.Broadcast(0.f);
	}
}

void UWaitCooldownChange::OnActiveEffectAdded(UAbilitySystemComponent* TargetASC, const FGameplayEffectSpec& Spec, FActiveGameplayEffectHandle ActiveEffectHandle)
{
	FGameplayTagContainer AssetTags;
	Spec.GetAllAssetTags(AssetTags);

	FGameplayTagContainer GrantedTags;
	Spec.GetAllGrantedTags(GrantedTags);

	if (AssetTags.HasTagExact(CooldownTag) || GrantedTags.HasTagExact(CooldownTag)) {
		FGameplayEffectQuery GameplayEffectQuery =
			FGameplayEffectQuery::MakeQuery_MatchAnyOwningTags(CooldownTag.GetSingleTagContainer());
		TArray<float> TimeRemaining = ASC->GetActiveEffectsTimeRemaining(GameplayEffectQuery);

		if (TimeRemaining.Num() > 0) {
			float MaxTimeRemaining = TimeRemaining[0];
			for (int i = 0; i < TimeRemaining.Num(); ++i) {
				if (TimeRemaining[i] > MaxTimeRemaining) {
					MaxTimeRemaining = TimeRemaining[0];
				}
			}
			CooldownBegin.Broadcast(MaxTimeRemaining);
		}
	}
}
