// Fill out your copyright notice in the Description page of Project Settings.


#include "Actor/PointCollectionActor.h"

#include "AbilitySystem/AuraAbilitySystemLibrary.h"
#include "Kismet/KismetMathLibrary.h"


APointCollectionActor::APointCollectionActor()
{
	PrimaryActorTick.bCanEverTick = false;

	Pt_0 = CreateDefaultSubobject<USceneComponent>("Pt_0");
	ImmutablePts.Add(Pt_0);
	SetRootComponent(Pt_0);
	
	Pt_1 = CreateDefaultSubobject<USceneComponent>("Pt_1");
	ImmutablePts.Add(Pt_1);
	Pt_1->SetupAttachment(GetRootComponent());
	
	Pt_2 = CreateDefaultSubobject<USceneComponent>("Pt_2");
	ImmutablePts.Add(Pt_2);
	Pt_2->SetupAttachment(GetRootComponent());
	
	Pt_3 = CreateDefaultSubobject<USceneComponent>("Pt_3");
	ImmutablePts.Add(Pt_3);
	Pt_3->SetupAttachment(GetRootComponent());

	Pt_4 = CreateDefaultSubobject<USceneComponent>("Pt_4");
	ImmutablePts.Add(Pt_4);
	Pt_4->SetupAttachment(GetRootComponent());
	
	Pt_5 = CreateDefaultSubobject<USceneComponent>("Pt_5");
	ImmutablePts.Add(Pt_5);
	Pt_5->SetupAttachment(GetRootComponent());
	
	Pt_6 = CreateDefaultSubobject<USceneComponent>("Pt_6");
	ImmutablePts.Add(Pt_6);
	Pt_6->SetupAttachment(GetRootComponent());
	
	Pt_7 = CreateDefaultSubobject<USceneComponent>("Pt_7");
	ImmutablePts.Add(Pt_7);
	Pt_7->SetupAttachment(GetRootComponent());
	
	Pt_8 = CreateDefaultSubobject<USceneComponent>("Pt_8");
	ImmutablePts.Add(Pt_8);
	Pt_8->SetupAttachment(GetRootComponent());
	

	Pt_9 = CreateDefaultSubobject<USceneComponent>("Pt_9");
	ImmutablePts.Add(Pt_9);
	Pt_9->SetupAttachment(GetRootComponent());
	

	Pt_10 = CreateDefaultSubobject<USceneComponent>("Pt_10");
	ImmutablePts.Add(Pt_10);
	Pt_10->SetupAttachment(GetRootComponent());
	
}

TArray<USceneComponent*> APointCollectionActor::GetGroundPoints(const FVector& GroundLocation, int32 NumPoints, float YawOverride)
{
	checkf(ImmutablePts.Num()>=NumPoints,TEXT("Immutable points cannot be zero"));

	TArray<USceneComponent*> ArrayPts;

	for (USceneComponent* Points : ImmutablePts) {
		if (ArrayPts.Num() >= NumPoints) return ArrayPts;

		if (Points != Pt_0) {
			FVector ToVector = Points->GetComponentLocation() - Pt_0->GetComponentLocation();
			ToVector = ToVector.RotateAngleAxis(YawOverride, FVector::UpVector);
			Points->SetWorldLocation(Pt_0->GetComponentLocation() + ToVector);
		}

		// Make the linear object to check the collision at z-axis
		const FVector Upper = FVector(
			Points->GetComponentLocation().X,
			Points->GetComponentLocation().Y, 
			Points->GetComponentLocation().Z + 500.f
		);
		FVector Lower = FVector(
			Points->GetComponentLocation().X,
			Points->GetComponentLocation().Y,
			Points->GetComponentLocation().Z - 500.f
		);

		TArray<AActor*> IgnoreActors;
		UAuraAbilitySystemLibrary::GetLivePlayersWithinRadius(
			this,
			IgnoreActors,
			TArray<AActor*>(),
			1500.f,
			GetActorLocation()
		);

		FHitResult HitResult;
		FCollisionQueryParams QueryParams;
		QueryParams.AddIgnoredActors(IgnoreActors);
		GetWorld()->LineTraceSingleByProfile(HitResult, Upper, Lower, FName("BlockAll"), QueryParams);

		const FVector AdjustedLocation = FVector(
			Points->GetComponentLocation().X, 
			Points->GetComponentLocation().Y, 
			HitResult.ImpactPoint.Z
		);
		Points->SetWorldLocation(AdjustedLocation);
		Points->SetWorldRotation(UKismetMathLibrary::MakeRotFromZ(HitResult.ImpactNormal));

		ArrayPts.Add(Points);
	}
	
	return ArrayPts;
}

void APointCollectionActor::BeginPlay()
{
	Super::BeginPlay();
	
}



