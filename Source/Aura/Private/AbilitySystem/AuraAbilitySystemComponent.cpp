// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/AuraAbilitySystemComponent.h"

#include "AbilitySystem/Abilities/AuraGameplayAbility.h"


void UAuraAbilitySystemComponent::AbilitySetInfo()
{
	OnGameplayEffectAppliedDelegateToSelf.AddUObject(this, &UAuraAbilitySystemComponent::ClientEffectApplied);

}

void UAuraAbilitySystemComponent::AddCharacterAbilities(const TArray<TSubclassOf<UGameplayAbility>>& StartUpAbilities)
{
	for (auto GameplayAbility : StartUpAbilities) {
		FGameplayAbilitySpec GASpec = FGameplayAbilitySpec(GameplayAbility, 1);
		if (const UAuraGameplayAbility* AuraGA = Cast<UAuraGameplayAbility>(GASpec.Ability)) {
			GASpec.DynamicAbilityTags.AddTag(AuraGA->StartupInputTag);
			GiveAbility(GASpec);
		}
	}
}

void UAuraAbilitySystemComponent::AbilityTagHeld(const FGameplayTag& Tag)
{
	if (!Tag.IsValid())return;

	for (auto& spec : GetActivatableAbilities()) {
		if (spec.DynamicAbilityTags.HasTagExact(Tag)) {
			AbilitySpecInputPressed(spec);
			if (!spec.IsActive()) {
				TryActivateAbility(spec.Handle);
			}
		}
	}
}

void UAuraAbilitySystemComponent::AbilityTagRelease(const FGameplayTag& Tag)
{
	if (!Tag.IsValid())return;

	for (auto& spec : GetActivatableAbilities()) {
		if (spec.DynamicAbilityTags.HasTagExact(Tag)) {
			AbilitySpecInputReleased(spec);
		}
	}
}

void UAuraAbilitySystemComponent::ClientEffectApplied_Implementation(UAbilitySystemComponent* ASC, const FGameplayEffectSpec& GameplayEffectSpec, FActiveGameplayEffectHandle GameplayEffectHandle)
{
	FGameplayTagContainer TagContainer;
	GameplayEffectSpec.GetAllAssetTags(TagContainer);

	EffectTags.Broadcast(TagContainer);
}
