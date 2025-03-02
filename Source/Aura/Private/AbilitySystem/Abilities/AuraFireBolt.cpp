// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Abilities/AuraFireBolt.h"

#include "AuraGameplayTags.h"
#include "Interaction/CombatInterface.h"
#include "AbilitySystem/AuraAbilitySystemLibrary.h"
#include "Actor/AuraProjectileActor.h"
#include "GameFramework/ProjectileMovementComponent.h"

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

void UAuraFireBolt::SpawnProjectiles(const FVector& ProjectileTargetLocation , const FGameplayTag& SocketTag ,bool bPitch, float PitchOverride, AActor* HomingTarget)
{
	const bool bIsServer = (GetAvatarActorFromActorInfo()->HasAuthority());
	if (!bIsServer) return;

	ICombatInterface* CombatInterface = Cast<ICombatInterface>(GetAvatarActorFromActorInfo());
	
	FVector WeaponSocketLocation = ICombatInterface::Execute_GetWeaponTipSocketLocation(GetAvatarActorFromActorInfo(), SocketTag);
	

	FRotator ForwardDirection = (ProjectileTargetLocation - WeaponSocketLocation).Rotation();
	if (bPitch) ForwardDirection.Pitch = PitchOverride;

	const FVector Forward = ForwardDirection.Vector();
	
	int32 EffectNumberProjectil = FMath::Min(NumProjectiles, GetAbilityLevel());
	TArray<FRotator> Rotators = UAuraAbilitySystemLibrary::SpawnRotateRotators(Forward, FVector::UpVector, EffectNumberProjectil, ProjectileSpread);

	const FVector Start = WeaponSocketLocation + FVector(0, 0, 5);
	for (auto& i : Rotators) {
		FTransform SpawnTransform;
		SpawnTransform.SetLocation(WeaponSocketLocation);
		SpawnTransform.SetRotation(i.Quaternion());

		AAuraProjectileActor* Projectile = GetWorld()->SpawnActorDeferred<AAuraProjectileActor>(
			ProjectileClass,
			SpawnTransform,
			GetOwningActorFromActorInfo(),
			Cast<APawn>(GetOwningActorFromActorInfo()),
			ESpawnActorCollisionHandlingMethod::AlwaysSpawn
		);
		if (HomingTarget->Implements<UCombatInterface>()) {
			Projectile->ProjectileMovement->HomingTargetComponent = HomingTarget->GetRootComponent();
		}
		else {
			HomingTargetSceneComponent = NewObject<USceneComponent>(USceneComponent::StaticClass());
			HomingTargetSceneComponent->SetWorldLocation(ProjectileTargetLocation);
			Projectile->ProjectileMovement->HomingTargetComponent = HomingTargetSceneComponent;	
		}
		Projectile->ProjectileMovement->HomingAccelerationMagnitude = FMath::FRandRange(HomingAccelerationMin, HomingAccelerationMax);
		Projectile->ProjectileMovement->bIsHomingProjectile = bHomingTarget;

		Projectile->DamageEffectParams = InitialDamageEffect(nullptr);
		Projectile->FinishSpawning(SpawnTransform);
	}
}
