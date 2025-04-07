// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Abilities/AuraBeamSpell.h"

#include "AuraGameplayTags.h"
#include "GameFramework/Character.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Interaction/CombatInterface.h"
#include "AbilitySystem/AuraAbilitySystemLibrary.h"

FString UAuraBeamSpell::GetCurrentDescription(int32 level)
{
	FAuraGameplayTags Tag = FAuraGameplayTags::Get();
	const int32 ScaledDamage = Damage.GetValueAtLevel(GetAbilityLevel());
	float ManaCost = GetManaCost(level);
	float CooldownTime = GetCooldown(level);


	return FString::Printf(TEXT("<Title>%s </>\n<Default>%s </><Damage>%d </><Default>%s </>\n\n<Default> Level</>: <Level>%d</>\n<Default> Mana cost</>: <ManaCost>%.1f</>\n<Default> Cooldown Time</>: <Cooldown>%.1f</>"),
		L"Lightning Shock",
		L"Lauch beam of lightning, trigger stuuned and cause continuos damage",
		ScaledDamage,
		L"Lightning with a chance to stunned.",
		level,
		ManaCost,
		CooldownTime
	);
}

FString UAuraBeamSpell::GetNextDescription(int32 level)
{
	FAuraGameplayTags Tag = FAuraGameplayTags::Get();
	const int32 ScaledDamage = Damage.GetValueAtLevel(GetAbilityLevel());
	float ManaCost = GetManaCost(level);
	float CooldownTime = GetCooldown(level);


	return FString::Printf(TEXT("<Title>%s </>\n<Default>%s </><Damage>%d </><Default>%s </>\n\n<Default> Level</>: <Level>%d</>\n<Default> Mana cost</>: <ManaCost>%.1f</>\n<Default> Cooldown Time</>: <Cooldown>%.1f</>"),
		L"Lightning Shock",
		L"Lauch beam of lightning, trigger stuuned and cause continuos damage",
		ScaledDamage,
		L"Lightning with a chance to stunned.",
		level,
		ManaCost,
		CooldownTime
	);
}

void UAuraBeamSpell::StoreOwnerVariables()
{
	if (CurrentActorInfo) {
		OwnerPlayerController = CurrentActorInfo->PlayerController.Get();
		OwnerCharacter = Cast<ACharacter>(CurrentActorInfo->AvatarActor);
	}
}

void UAuraBeamSpell::StoreCursorHitTarget(const FHitResult& HitResult)
{
	if (HitResult.bBlockingHit) {
		CursorHitLocation = HitResult.ImpactPoint;
		CursorHitActor = HitResult.GetActor();
	}
	else {
		CancelAbility(CurrentSpecHandle, CurrentActorInfo,CurrentActivationInfo,true);
	}
}

void UAuraBeamSpell::TraceFirstTarget(const FVector TargetLocation)
{
	check(OwnerCharacter);
	if (OwnerCharacter->Implements<UCombatInterface>()) {
		if (USkeletalMeshComponent* Weapon = ICombatInterface::Execute_GetWeapon(OwnerCharacter)) {
			const FVector WeaponSocketLocation = Weapon->GetSocketLocation(FName("TipSocket"));

			TArray<AActor*> ActorToIgnore;
			ActorToIgnore.Add(OwnerCharacter);
			FHitResult HitResult;

			UKismetSystemLibrary::SphereTraceSingle(
				OwnerCharacter,
				WeaponSocketLocation,
				TargetLocation,
				10.f,
				TraceTypeQuery1,
				false,
				ActorToIgnore,
				EDrawDebugTrace::None,
				HitResult,
				true
			);

			if (HitResult.bBlockingHit){
				CursorHitActor = HitResult.GetActor();
				CursorHitLocation = HitResult.ImpactPoint;
			}
		}
	}
	if (ICombatInterface* CombatInterface = Cast<ICombatInterface>(CursorHitActor)){
		if (CombatInterface->GetOnDeathDelegate().IsAlreadyBound(this, &UAuraBeamSpell::PrimaryTargetDeath)) {
			return;
		}
		CombatInterface->GetOnDeathDelegate().AddDynamic(this, &UAuraBeamSpell::PrimaryTargetDeath);
	}
}

void UAuraBeamSpell::StoreAdditionalTarget(TArray<AActor*>& OtherActors)
{
	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.Add(GetAvatarActorFromActorInfo());
	ActorsToIgnore.Add(CursorHitActor);

	TArray<AActor*> AllNearestActors;

	// Find all actor in the radius of the hit location and output to OutTarget.
	UAuraAbilitySystemLibrary::GetLivePlayersWithinRadius(
		GetAvatarActorFromActorInfo(),
		AllNearestActors,
		ActorsToIgnore,
		850.f,
		CursorHitLocation
	);

	int32 NumOfTarget = 5;

	UAuraAbilitySystemLibrary::FindNumOfNearActos(
		NumOfTarget,
		AllNearestActors,
		OtherActors,
		CursorHitLocation
	);
	
	for (auto actor : OtherActors) {
		if (ICombatInterface* CombatInterface = Cast<ICombatInterface>(actor)) {
			if (CombatInterface->GetOnDeathDelegate().IsAlreadyBound(this, &UAuraBeamSpell::AdditionalTargetDeath)) {
				return;
			}
			CombatInterface->GetOnDeathDelegate().AddDynamic(this, &UAuraBeamSpell::AdditionalTargetDeath);
		}
	}
}
