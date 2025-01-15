// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/AuraCharacterBase.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "Components/CapsuleComponent.h"
#include "Aura/Aura.h"
#include "AuraGameplayTags.h"

// Sets default values
AAuraCharacterBase::AAuraCharacterBase()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
	GetMesh()->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
	GetMesh()->SetCollisionResponseToChannel(ECC_Projectile, ECR_Overlap);
	GetMesh()->SetGenerateOverlapEvents(true);

	Weapon = CreateDefaultSubobject<USkeletalMeshComponent>(FName("Weapon"));
	Weapon->SetupAttachment(GetMesh(), FName("WeaponHandSocket"));
	Weapon->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

UAbilitySystemComponent* AAuraCharacterBase::GetAbilitySystemComponent() const
{
	return AbilitySystemComp;
}

void AAuraCharacterBase::InitActorInfo()
{
}

FVector AAuraCharacterBase::GetPartSocketLocation()
{
	check(Weapon);
	return Weapon->GetSocketLocation(WeaponTip);
}



void AAuraCharacterBase::ApplyDefaultAttribute(TSubclassOf<UGameplayEffect> DefaultAttribute, float level)
{
	check(IsValid(GetAbilitySystemComponent()));
	check(DefaultAttribute);

	FGameplayEffectContextHandle ContextHandle =
		GetAbilitySystemComponent()->MakeEffectContext();

	ContextHandle.AddSourceObject(this);

	const FGameplayEffectSpecHandle SpecHandle =
		GetAbilitySystemComponent()->MakeOutgoingSpec(DefaultAttribute, 1.f, ContextHandle);

	GetAbilitySystemComponent()->ApplyGameplayEffectSpecToTarget(*SpecHandle.Data.Get(), GetAbilitySystemComponent());
}

void AAuraCharacterBase::InitDefaultAttributes()
{
	ApplyDefaultAttribute(DefaultPrimaryAttribute, 1.f);
	ApplyDefaultAttribute(DefaultSecondaryAttribute, 1.f);
	ApplyDefaultAttribute(DefaultVitalAttribute, 1.f);
}

void AAuraCharacterBase::AddCharacterAbilities()
{
	UAuraAbilitySystemComponent* AuraASC = CastChecked<UAuraAbilitySystemComponent>(AbilitySystemComp);
	if (!HasAuthority()) {
		return;
	}

	AuraASC->AddCharacterAbilities(StartUpAbilities);
}




// Called when the game starts or when spawned
void AAuraCharacterBase::BeginPlay()
{
	Super::BeginPlay();
	
}

UAnimMontage* AAuraCharacterBase::GetHitReactAnimMontage_Implementation()
{
	return AnimMontage;
}

void AAuraCharacterBase::Die()
{
	Weapon->DetachFromComponent(FDetachmentTransformRules(EDetachmentRule::KeepWorld, true));
	MulticastHandleDeath();
}

FVector AAuraCharacterBase::GetWeaponTipSocketLocation_Implementation(const FGameplayTag& MontageTag)
{
	FAuraGameplayTags AuraTag = FAuraGameplayTags::Get();
	if (MontageTag.MatchesTagExact(AuraTag.Montage_Attack_Weapon)) {
		return Weapon->GetSocketLocation(WeaponTip);
	}
	else if (MontageTag.MatchesTagExact(AuraTag.Montage_Attack_LeftHand)) {
		return GetMesh()->GetSocketLocation(LeftHand);
	}
	else if (MontageTag.MatchesTagExact(AuraTag.Montage_Attack_RightHand)){
		return GetMesh()->GetSocketLocation(RightHand);
	}

	return FVector();
}

bool AAuraCharacterBase::bIsDead_Implementation() const
{
	return IsDead;
}

AActor* AAuraCharacterBase::GetAvatarActor_Implementation()
{
	return this;
}

TArray<FTagMontage> AAuraCharacterBase::GetAttackMontages_Implementation()
{
	return AttackMontages;
}

void AAuraCharacterBase::MulticastHandleDeath_Implementation()
{
	Weapon->SetSimulatePhysics(true);
	Weapon->SetEnableGravity(true);
	Weapon->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);

	GetMesh()->SetSimulatePhysics(true);
	GetMesh()->SetEnableGravity(true);
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
	GetMesh()->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Block);

	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	Disslove();
	IsDead = true;
}

void AAuraCharacterBase::Disslove()
{
	if (IsValid(DynamicMaterialDisslove)) {
		UMaterialInstanceDynamic* Material = UMaterialInstanceDynamic::Create(DynamicMaterialDisslove, this);
		GetMesh()->SetMaterial(0, Material);
		StartDissloveEvent(Material);
	}
	
	if (IsValid(DynamicMaterialWeaponDisslove)) {
		UMaterialInstanceDynamic* Material_Weapon = UMaterialInstanceDynamic::Create(DynamicMaterialWeaponDisslove, this);
		Weapon->SetMaterial(0, Material_Weapon);
		StartWeaponDissloveEvent(Material_Weapon);
	}
}
