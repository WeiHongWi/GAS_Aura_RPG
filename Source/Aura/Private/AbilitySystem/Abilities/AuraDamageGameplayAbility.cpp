// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Abilities/AuraDamageGameplayAbility.h"

#include "Aura/Public/AbilitySystem/AuraAbilitySystemLibrary.h"
#include "AbilitySystemComponent.h"
#include "AuraGameplayTags.h"

void UAuraDamageGameplayAbility::CauseDamage(AActor* Target)
{
	const FGameplayEffectSpecHandle EffectSpecHandle = 
		MakeOutgoingGameplayEffectSpec(DamageEffectClass, 1);

	for (auto& pair : DamageTypes) {
		const float scaled_damage = pair.Value.GetValueAtLevel(1);
		UAuraAbilitySystemLibrary::AssignTagSetByCallerMagnitude(
			EffectSpecHandle,
			pair.Key,
			scaled_damage
		);
	}
	GetAbilitySystemComponentFromActorInfo()->ApplyGameplayEffectSpecToTarget(
		*EffectSpecHandle.Data.Get(),
		UAuraAbilitySystemLibrary::GetAbilitySystemComponent(Target)
	);
}

float UAuraDamageGameplayAbility::GetDamageByDamageType(float InLevel, const FGameplayTag DamageType)
{
	checkf(DamageTypes.Contains(DamageType), TEXT("The declaration of damage type doesn't contain this [%s]"), *DamageType.ToString());
	return DamageTypes[DamageType].GetValueAtLevel(InLevel);
}
