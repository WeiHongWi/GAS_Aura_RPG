// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Abilities/AuraArcaneShard.h"

#include "AuraGameplayTags.h"

FString UAuraArcaneShard::GetCurrentDescription(int32 level)
{
	FAuraGameplayTags Tag = FAuraGameplayTags::Get();
	const int32 ScaledDamage = Damage.GetValueAtLevel(GetAbilityLevel());
	float ManaCost = GetManaCost(level);
	float CooldownTime = GetCooldown(level);


	return FString::Printf(TEXT("<Title>%s </>\n<Default>%s </><Damage>%d </><Default>%s </>\n\n<Default> Level</>: <Level>%d</>\n<Default> Mana cost</>: <ManaCost>%.1f</>\n<Default> Cooldown Time</>: <Cooldown>%.1f</>"),
		L"Arcane Shard",
		L"Lauch shard of arcane, area of effect cause",
		ScaledDamage,
		L"damage with a lot of shard.",
		level,
		ManaCost,
		CooldownTime
	);
}

FString UAuraArcaneShard::GetNextDescription(int32 level)
{
	FAuraGameplayTags Tag = FAuraGameplayTags::Get();
	const int32 ScaledDamage = Damage.GetValueAtLevel(GetAbilityLevel());
	float ManaCost = GetManaCost(level);
	float CooldownTime = GetCooldown(level);


	return FString::Printf(TEXT("<Title>%s </>\n<Default>%s </><Damage>%d </><Default>%s </>\n\n<Default> Level</>: <Level>%d</>\n<Default> Mana cost</>: <ManaCost>%.1f</>\n<Default> Cooldown Time</>: <Cooldown>%.1f</>"),
		L"Arcane Shard",
		L"Lauch shard of arcane, area of effect cause",
		ScaledDamage,
		L"damage with a lot of shard.",
		level,
		ManaCost,
		CooldownTime
	);
}
