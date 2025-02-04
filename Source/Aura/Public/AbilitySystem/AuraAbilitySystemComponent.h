// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "AuraAbilitySystemComponent.generated.h"


DECLARE_MULTICAST_DELEGATE_OneParam(FEffectTags,FGameplayTagContainer&);
DECLARE_MULTICAST_DELEGATE_OneParam(FAbilityInfo, UAuraAbilitySystemComponent*);
DECLARE_DELEGATE_OneParam(FForEachAbility, const FGameplayAbilitySpec&);
/**
 * 
 */
UCLASS()
class AURA_API UAuraAbilitySystemComponent : public UAbilitySystemComponent
{
	GENERATED_BODY()

public:
	void AbilitySetInfo();

	bool bIsStartupAbilityInitialized = false;

	FEffectTags EffectTags;
	FAbilityInfo AbilityInfoDelegate;

	void AddCharacterAbilities(const TArray<TSubclassOf<UGameplayAbility>>& StartUpAbilities);
	void AddCharacterPassiveAbilities(const TArray<TSubclassOf<UGameplayAbility>>& StartUpPassivepAbilities);

	void AbilityTagHeld(const FGameplayTag& Tag);
	void AbilityTagRelease(const FGameplayTag& Tag);
	void ForEachAbility(const FForEachAbility& Delegate);

	static FGameplayTag GetAbilityTagBySpec(const FGameplayAbilitySpec& AbilitySpec);
	static FGameplayTag GetInputTagBySpec(const FGameplayAbilitySpec& AbilitySpec);

protected:
	UFUNCTION(Client,Reliable)
	void ClientEffectApplied(UAbilitySystemComponent* ASC
					   , const FGameplayEffectSpec& GameplayEffectSpec
					   , FActiveGameplayEffectHandle GameplayEffectHandle
	);
	virtual void OnRep_ActivateAbilities() override;
};
