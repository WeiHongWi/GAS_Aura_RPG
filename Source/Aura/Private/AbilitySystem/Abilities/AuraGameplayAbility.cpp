// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Abilities/AuraGameplayAbility.h"

#include "AbilitySystem/AuraAttributeSet.h"

FString UAuraGameplayAbility::GetCurrentDescription(int32 level)
{
	return FString::Printf(TEXT("<Default>%s, </><Level>%d</>"), L"Default Ability Name - LoremIpsum LoremIpsum LoremIpsum LoremIpsum LoremIpsum LoremIpsum LoremIpsum LoremIpsum LoremIpsum LoremIpsum LoremIpsum LoremIpsum LoremIpsum LoremIpsum LoremIpsum LoremIpsum", level);
}

FString UAuraGameplayAbility::GetNextDescription(int32 level)
{
	return FString::Printf(TEXT("<Default>Next Level: </><Level>%d</> \n<Default>Causes much more damage. </>"), level);
}

FString UAuraGameplayAbility::GetLockedSpellDescription(int32 level)
{
	return FString::Printf(TEXT("<Default>Spell Locked Until Level: %d</>"), level);
}

float UAuraGameplayAbility::GetManaCost(float InLevel)
{
	float ManaCost = 0.f;
	if (UGameplayEffect* GameplayEffect = GetCostGameplayEffect()) {
		for (FGameplayModifierInfo modifier : GameplayEffect->Modifiers) {
			if (modifier.Attribute == UAuraAttributeSet::GetManaAttribute()) {
				modifier.ModifierMagnitude.GetStaticMagnitudeIfPossible(InLevel, ManaCost);
				break;
			}
		}
	}

	return ManaCost;
}

float UAuraGameplayAbility::GetCooldown(float InLevel)
{
	float CooldownTime = 0.f;
	if (UGameplayEffect* CooldownEffect = GetCooldownGameplayEffect()) {
		CooldownEffect->DurationMagnitude.GetStaticMagnitudeIfPossible(InLevel, CooldownTime);
	}
	return CooldownTime;
}
