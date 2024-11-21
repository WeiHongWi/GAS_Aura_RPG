// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "AuraAbilitySystemComponent.generated.h"


DECLARE_MULTICAST_DELEGATE_OneParam(FEffectTags,FGameplayTagContainer&);
/**
 * 
 */
UCLASS()
class AURA_API UAuraAbilitySystemComponent : public UAbilitySystemComponent
{
	GENERATED_BODY()

public:
	void AbilitySetInfo();

	FEffectTags EffectTags;

	void AddCharacterAbilities(const TArray<TSubclassOf<UGameplayAbility>>& StartUpAbilities);

	void AbilityTagHeld(const FGameplayTag& Tag);
	void AbilityTagRelease(const FGameplayTag& Tag);

protected:
	UFUNCTION(Client,Reliable)
	void ClientEffectApplied(UAbilitySystemComponent* ASC
					   , const FGameplayEffectSpec& GameplayEffectSpec
					   , FActiveGameplayEffectHandle GameplayEffectHandle);
	
};
