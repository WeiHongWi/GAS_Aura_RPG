// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/AuraAbilitySystemComponent.h"



void UAuraAbilitySystemComponent::AbilitySetInfo()
{
	OnGameplayEffectAppliedDelegateToSelf.AddUObject(this, &UAuraAbilitySystemComponent::EffectApplied);

}

void UAuraAbilitySystemComponent::AddCharacterAbilities(const TArray<TSubclassOf<UGameplayAbility>>& StartUpAbilities)
{
	for (auto GameplayAbility : StartUpAbilities) {
		FGameplayAbilitySpec GameplayAbilitySpec = FGameplayAbilitySpec(GameplayAbility, 1);
		//GiveAbility(GameplayAbilitySpec);
		GiveAbilityAndActivateOnce(GameplayAbilitySpec);
	}
}

void UAuraAbilitySystemComponent::EffectApplied(UAbilitySystemComponent* ASC, const FGameplayEffectSpec& GameplayEffectSpec, FActiveGameplayEffectHandle GameplayEffectHandle)
{
	FGameplayTagContainer TagContainer;
	GameplayEffectSpec.GetAllAssetTags(TagContainer);

	EffectTags.Broadcast(TagContainer);
}
