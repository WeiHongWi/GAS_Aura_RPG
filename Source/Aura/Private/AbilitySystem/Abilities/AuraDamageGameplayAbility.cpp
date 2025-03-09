// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Abilities/AuraDamageGameplayAbility.h"

#include "Aura/Public/AbilitySystem/AuraAbilitySystemLibrary.h"
#include "AbilitySystemComponent.h"
#include "AuraGameplayTags.h"
#include "AuraAbilityTypes.h"
#include "AbilitySystemBlueprintLibrary.h"

void UAuraDamageGameplayAbility::CauseDamage(AActor* Target)
{
	const FGameplayEffectSpecHandle EffectSpecHandle = 
		MakeOutgoingGameplayEffectSpec(DamageEffectClass, 1);

	const float scaled_damage = Damage.GetValueAtLevel(1);
	UAuraAbilitySystemLibrary::AssignTagSetByCallerMagnitude(
		EffectSpecHandle,
		DamageType,
		scaled_damage
	);

	GetAbilitySystemComponentFromActorInfo()->ApplyGameplayEffectSpecToTarget(
		*EffectSpecHandle.Data.Get(),
		UAuraAbilitySystemLibrary::GetAbilitySystemComponent(Target)
	);
}

float UAuraDamageGameplayAbility::GetDamageByLevel() const
{
	return Damage.GetValueAtLevel(GetAbilityLevel());
}

FDamageEffectParams UAuraDamageGameplayAbility::InitialDamageEffect(AActor* TargetActor) const
{
	FDamageEffectParams Params;
	Params.WorldContextObject = GetAvatarActorFromActorInfo();

	Params.SourceASC = GetAbilitySystemComponentFromActorInfo();
	Params.TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetActor);
	Params.DamageGameplayEffectClass = DamageEffectClass;
	
	Params.Base_Damage = Damage.GetValueAtLevel(GetAbilityLevel());
	Params.Ability_Level = GetAbilityLevel();
	Params.DamageType = DamageType;
	
	Params.Debuff_Chance = DebuffChance;
	Params.Debuff_Damage = DebuffDamage;
	Params.Debuff_Frequency = DebuffFrequency;
	Params.Debuff_Duration = DebuffDuration;
	Params.DeathImpulseMagnitude = DeathImpulseMagnitude;

	Params.KnockbackForceMagnitude = KnockbackForceMagnitude;
	Params.KnockbackChance = KnockbackChance;
	if (IsValid(TargetActor))
	{
		FRotator Rotation = (TargetActor->GetActorLocation() - GetAvatarActorFromActorInfo()->GetActorLocation()).Rotation();
		Rotation.Pitch = 45.f;
		const FVector ToTarget = Rotation.Vector();
		Params.DeathImpulse = ToTarget * DeathImpulseMagnitude;
		Params.KnockbackForce = ToTarget * KnockbackForceMagnitude;
	}


	return Params;
}

