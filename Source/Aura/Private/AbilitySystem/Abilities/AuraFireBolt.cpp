// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Abilities/AuraFireBolt.h"
#include "AuraGameplayTags.h"

FString UAuraFireBolt::GetCurrentDescription(int32 level)
{
	FAuraGameplayTags Tag = FAuraGameplayTags::Get();
	const int32 ScaledDamage = Damage.GetValueAtLevel(GetAbilityLevel());
	float ManaCost = GetManaCost(level);
	float CooldownTime = GetCooldown(level);
	
	if (level == 1) {
		return FString::Printf(TEXT("<Title>%s </>\n<Default>%s </><Damage>%d </><Default>%s </>\n\n<Default> Level</>: <Level>%d</>\n<Default> Mana cost</>: <ManaCost>%.1f</>\n<Default> Cooldown Time</>: <Cooldown>%.1f</>"),
			L"Fire Bolt",
			L"Lauch a bolt of fire, exploding on impact and dealing",
			ScaledDamage,
			L"fire damage with a chance to burn.",
			level,
			ManaCost,
			CooldownTime
		);
	}
	else {
		return FString::Printf(TEXT("<Title>%s </>\n<Default>%s </><Default>%d </><Default>%s </><Damage>%d </><Default>%s </>\n\n<Default> Level</>: <Level>%d</>\n<Default> Mana cost</>: <ManaCost>%.1f</>\n<Default> Cooldown Time</>: <Cooldown>%.1f</>"),
			L"Fire Bolt",
			L"Lauch",
			FMath::Min(level, NumProjectiles),
			L"bolt of fire, exploding on impact and dealing",
			ScaledDamage,
			L"fire damage with a chance to burn.",
			level,
			ManaCost,
			CooldownTime
		);
	}

}

FString UAuraFireBolt::GetNextDescription(int32 level)
{
	FAuraGameplayTags Tag = FAuraGameplayTags::Get();
	const int32 ScaledDamage = Damage.GetValueAtLevel(GetAbilityLevel());
	float ManaCost = GetManaCost(level);
	float CooldownTime = GetCooldown(level);

	return FString::Printf(TEXT("<Title>%s </>\n<Default>%s </><Default>%d </><Default>%s </><Damage>%d </><Default>%s </>\n\n<Default> Level</>: <Level>%d</>\n<Default> Mana cost</>: <ManaCost>%.1f</>\n<Default> Cooldown Time</>: <Cooldown>%.1f</>"),
		L"Fire Bolt - Next Level",
		L"Lauch",
		FMath::Min(level, NumProjectiles),
		L"bolt of fire, exploding on impact and dealing",
		ScaledDamage,
		L"fire damage with a chance to burn.",
		level,
		ManaCost,
		CooldownTime
	);
}