// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Abilities/AuraSummonAbility.h"

#include "Kismet/KismetSystemLibrary.h"

TArray<FVector> UAuraSummonAbility::GetSpawnLocation()
{
	const FVector Forward = GetAvatarActorFromActorInfo()->GetActorForwardVector();
	const FVector Location = GetAvatarActorFromActorInfo()->GetActorLocation();

	const FVector RightToSpread = Forward.RotateAngleAxis(Spread / 2.f, FVector::UpVector);
	const FVector LeftToSpread = Forward.RotateAngleAxis(-Spread / 2.f, FVector::UpVector);

	float SpreadDelta = Spread / NumMinions;
	TArray<FVector> SpawnLocation;

	for (int i = 0; i < NumMinions; ++i) {
		const FVector DeltaSpread = RightToSpread.RotateAngleAxis(-SpreadDelta*i, FVector::UpVector);
		FVector SpreadLocation = Location + DeltaSpread * FMath::FRandRange(MinSpawnDistance, MaxSpawnDistance);
		
		FHitResult Hit;
		GetWorld()->LineTraceSingleByChannel(
			Hit,
			SpreadLocation + FVector(0, 0, 400.f),
			SpreadLocation - FVector(0, 0, 400.f),
			ECC_Visibility
		);
		if (Hit.bBlockingHit) {
			SpreadLocation = Hit.ImpactPoint;
		}

		SpawnLocation.Add(SpreadLocation);
	}

	return SpawnLocation;

}

TSubclassOf<APawn> UAuraSummonAbility::GetRandomMinion()
{
	const int index = FMath::RandRange(0, MinionClass.Num() - 1);
	return MinionClass[index];
}
