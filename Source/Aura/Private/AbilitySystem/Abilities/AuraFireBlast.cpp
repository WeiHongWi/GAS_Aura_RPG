// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Abilities/AuraFireBlast.h"

#include "AuraGameplayTags.h"
#include "AbilitySystem/Abilities/AuraFireBolt.h"
#include "AbilitySystem/AuraAbilitySystemLibrary.h"
#include "Actor/AuraFireBall.h"


FString UAuraFireBlast::GetCurrentDescription(int32 level)
{
	FAuraGameplayTags Tag = FAuraGameplayTags::Get();
	const int32 ScaledDamage = Damage.GetValueAtLevel(GetAbilityLevel());
	float ManaCost = GetManaCost(level);
	float CooldownTime = GetCooldown(level);

	return FString::Printf(TEXT("<Title>%s </>\n<Default>%s </><Default>%d </><Default>%s </><Damage>%d </><Default>%s </>\n\n<Default> Level</>: <Level>%d</>\n<Default> Mana cost</>: <ManaCost>%.1f</>\n<Default> Cooldown Time</>: <Cooldown>%.1f</>"),
		L"Fire Blast",
		L"Lauch",
		FMath::Min(level, NumProjectiles),
		L"fire balls in all directions, exploding on impact and dealing",
		ScaledDamage,
		L"fire damage with a chance to burn.",
		level,
		ManaCost,
		CooldownTime
	);
}

FString UAuraFireBlast::GetNextDescription(int32 level)
{
	FAuraGameplayTags Tag = FAuraGameplayTags::Get();
	const int32 ScaledDamage = Damage.GetValueAtLevel(GetAbilityLevel());
	float ManaCost = GetManaCost(level);
	float CooldownTime = GetCooldown(level);

	return FString::Printf(TEXT("<Title>%s </>\n<Default>%s </><Default>%d </><Default>%s </><Damage>%d </><Default>%s </>\n\n<Default> Level</>: <Level>%d</>\n<Default> Mana cost</>: <ManaCost>%.1f</>\n<Default> Cooldown Time</>: <Cooldown>%.1f</>"),
		L"Fire Blast - Next Level",
		L"Lauch",
		FMath::Min(level, NumProjectiles),
		L"fire balls in all directions, exploding on impact and dealing",
		ScaledDamage,
		L"fire damage with a chance to burn.",
		level,
		ManaCost,
		CooldownTime
	);
}

TArray<AAuraFireBall*> UAuraFireBlast::SpawnFireBalls()
{
	TArray<AAuraFireBall*> FireBalls;
	const FVector Forward = GetAvatarActorFromActorInfo()->GetActorForwardVector();
	const FVector Location = GetAvatarActorFromActorInfo()->GetActorLocation();
	TArray<FRotator> AllSpawnFireBall = UAuraAbilitySystemLibrary::SpawnRotateRotators(Forward, FVector::UpVector, NumProjectiles, 360.f);
	
	for (FRotator& rotator : AllSpawnFireBall) {
		FTransform Trans;
		Trans.SetLocation(Location);
		Trans.SetRotation(rotator.Quaternion());

		AAuraFireBall* fireball = GetWorld()->SpawnActorDeferred<AAuraFireBall>(
			FireBallClass,
			Trans,
			GetOwningActorFromActorInfo(),
			CurrentActorInfo->PlayerController->GetPawn(),
			ESpawnActorCollisionHandlingMethod::AlwaysSpawn
		);
		fireball->DamageEffectParams = InitialDamageEffect(nullptr);
		fireball->ReturningActor = GetAvatarActorFromActorInfo();

		fireball->ExplosionDamageEffectParams = InitialDamageEffect(nullptr);
		fireball->SetOwner(GetAvatarActorFromActorInfo());

		FireBalls.Add(fireball);
		fireball->FinishSpawning(Trans);
	}

	return FireBalls;
}
