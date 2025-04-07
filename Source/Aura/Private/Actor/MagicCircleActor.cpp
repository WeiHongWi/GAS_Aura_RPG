// Fill out your copyright notice in the Description page of Project Settings.


#include "Actor/MagicCircleActor.h"

#include "Components/DecalComponent.h"

// Sets default values
AMagicCircleActor::AMagicCircleActor()
{
	PrimaryActorTick.bCanEverTick = true;

	MagicCircleDecal = CreateDefaultSubobject<UDecalComponent>("MagicDecalComp");
	MagicCircleDecal->SetupAttachment(GetRootComponent());
}

// Called when the game starts or when spawned
void AMagicCircleActor::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AMagicCircleActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

