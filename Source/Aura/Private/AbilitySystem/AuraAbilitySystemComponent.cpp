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
	bIsStartupAbilityInitialized = true;
	AbilityInfoDelegate.Broadcast(this);
}

void UAuraAbilitySystemComponent::AddCharacterPassiveAbilities(const TArray<TSubclassOf<UGameplayAbility>>& StartUpPassivepAbilities)
{
	for (auto GameplayAbility : StartUpPassivepAbilities) {
		FGameplayAbilitySpec GASpec = FGameplayAbilitySpec(GameplayAbility, 1);
		GiveAbilityAndActivateOnce(GASpec);
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

void UAuraAbilitySystemComponent::ForEachAbility(const FForEachAbility& Delegate)
{
	FScopedAbilityListLock ActiveScopeLock(*this);
	for (const FGameplayAbilitySpec& AbilitySpec : GetActivatableAbilities()) {
		if (!Delegate.ExecuteIfBound(AbilitySpec)) {
			UE_LOG(LogTemp, Warning, TEXT("Failed to execute the delegate in %s."), *GetNameSafe(this));
		}
	}
}

FGameplayTag UAuraAbilitySystemComponent::GetAbilityTagBySpec(const FGameplayAbilitySpec& AbilitySpec)
{
	if (AbilitySpec.Ability) {
		for (FGameplayTag tag : AbilitySpec.Ability.Get()->AbilityTags) {
			if (tag.MatchesTag(FGameplayTag::RequestGameplayTag(FName("Abilities")))) {
				return tag;
			}
		}
	}
	return FGameplayTag();
}

FGameplayTag UAuraAbilitySystemComponent::GetInputTagBySpec(const FGameplayAbilitySpec& AbilitySpec)
{
	for (FGameplayTag tag : AbilitySpec.DynamicAbilityTags) {
		if(tag.MatchesTag(FGameplayTag::RequestGameplayTag(FName("InputTag")))) {
			return tag;
		}
	}
	return FGameplayTag();
}

void UAuraAbilitySystemComponent::OnRep_ActivateAbilities()
{
	Super::OnRep_ActivateAbilities();

	if (!bIsStartupAbilityInitialized) {
		bIsStartupAbilityInitialized = true;
		AbilityInfoDelegate.Broadcast(this);
	}
}

void UAuraAbilitySystemComponent::ClientEffectApplied_Implementation(UAbilitySystemComponent* ASC, const FGameplayEffectSpec& GameplayEffectSpec, FActiveGameplayEffectHandle GameplayEffectHandle)
{
	FGameplayTagContainer TagContainer;
	GameplayEffectSpec.GetAllAssetTags(TagContainer);

	EffectTags.Broadcast(TagContainer);
}
