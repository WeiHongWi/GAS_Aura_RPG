// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Abilities/AuraDamageGameplayAbility.h"

#include "Aura/Public/AbilitySystem/AuraAbilitySystemLibrary.h"
#include "AbilitySystemComponent.h"

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
