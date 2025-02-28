// Fill out your copyright notice in the Description page of Project Settings.


#include "Actor/AuraProjectileActor.h"

#include "Components/SphereComponent.h"
#include "Components/AudioComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "NiagaraFunctionLibrary.h"
#include "Aura/Aura.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemBlueprintLibrary.h"

#include "Aura/Public/AbilitySystem/AuraAbilitySystemLibrary.h"


void AAuraProjectileActor::OnHit()
{
	UGameplayStatics::PlaySoundAtLocation(this, SoundEffect, GetActorLocation(), FRotator::ZeroRotator);
	UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, ImpactEffect, GetActorLocation());
	if (LoopingSoundComponent) {
		LoopingSoundComponent->Stop();
	}
	bHit = true;
}

// Sets default values
AAuraProjectileActor::AAuraProjectileActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;

	Sphere = CreateDefaultSubobject<USphereComponent>("Sphere");
	SetRootComponent(Sphere);
	Sphere->SetCollisionObjectType(ECC_Projectile);
	Sphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	Sphere->SetCollisionResponseToAllChannels(ECR_Ignore);
	Sphere->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Overlap);
	Sphere->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Overlap);
	Sphere->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);

	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>("ProjectileMovement");
	ProjectileMovement->InitialSpeed = 550.f;
	ProjectileMovement->MaxSpeed = 550.f;
	ProjectileMovement->ProjectileGravityScale = 0.f;
}
// Called when the game starts or when spawned
void AAuraProjectileActor::BeginPlay()
{
	Super::BeginPlay();
	SetLifeSpan(LifeSpan);

	Sphere->OnComponentBeginOverlap.AddDynamic(this, &AAuraProjectileActor::OnSphereOvelap);
	LoopingSoundComponent = UGameplayStatics::SpawnSoundAttached(LoopingSound, GetRootComponent());
}

void AAuraProjectileActor::OnSphereOvelap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	AActor* SourceActor = DamageEffectParams.SourceASC->GetAvatarActor();
	AActor* PlayerToEnemy = Cast<AActor>(GetInstigator());

	if (SourceActor == OtherActor) return;
	if (IsValid(PlayerToEnemy) && UAuraAbilitySystemLibrary::IsFriend(PlayerToEnemy, OtherActor))return;
	if (!bHit) OnHit();

	if (HasAuthority()) {
		if (UAbilitySystemComponent* TargetASC = 
			UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(OtherActor)){
			DamageEffectParams.TargetASC = TargetASC;
			UAuraAbilitySystemLibrary::ApplyDamageEffect(DamageEffectParams);
		}
		Destroy();
	}
	else {
		bHit = true;
	}
}

void AAuraProjectileActor::Destroyed()
{
	if (!bHit && !HasAuthority()) OnHit();
	Super::Destroyed();
}




