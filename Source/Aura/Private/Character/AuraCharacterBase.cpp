// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/AuraCharacterBase.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "Components/CapsuleComponent.h"
#include "Aura/Aura.h"
#include "AuraGameplayTags.h"
#include "Kismet/GameplayStatics.h"
#include "AbilitySystem/Debuff/DebuffNiagaraComp.h"

// Sets default values
AAuraCharacterBase::AAuraCharacterBase()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	BurnNiagaraComp =  CreateDefaultSubobject<UDebuffNiagaraComp>("BurnNiagaraComp");
	BurnNiagaraComp->SetupAttachment(GetRootComponent());
	BurnNiagaraComp->DebuffTag = FAuraGameplayTags::Get().Debuff_Burn;

	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
	GetMesh()->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
	GetMesh()->SetCollisionResponseToChannel(ECC_Projectile, ECR_Overlap);
	GetMesh()->SetGenerateOverlapEvents(true);

	Weapon = CreateDefaultSubobject<USkeletalMeshComponent>(FName("Weapon"));
	Weapon->SetupAttachment(GetMesh(), FName("WeaponHandSocket"));
	Weapon->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

FOnASCRegistered AAuraCharacterBase::GetOnASCRegisterDelegate()
{
	return OnAscRegistered;
}

FOnDeath AAuraCharacterBase::GetOnDeathDelegate()
{
	return OnDeath;
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
	AuraASC->AddCharacterPassiveAbilities(StartUpPassiveAbilities);
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
	if (MontageTag.MatchesTagExact(AuraTag.CombatSocket_Weapon)) {
		return Weapon->GetSocketLocation(WeaponTip);
	}
	if (MontageTag.MatchesTagExact(AuraTag.CombatSocket_LeftHand)) {
		return GetMesh()->GetSocketLocation(LeftHand);
	}
	if (MontageTag.MatchesTagExact(AuraTag.CombatSocket_RightHand)){
		return GetMesh()->GetSocketLocation(RightHand);
	}
	if (MontageTag.MatchesTagExact(AuraTag.CombatSocket_Tail)) {
		return GetMesh()->GetSocketLocation(TailSocket);
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

UNiagaraSystem* AAuraCharacterBase::GetBloodEffect_Implementation()
{
	return BloodEffect;
}

FTagMontage AAuraCharacterBase::GetTagMontageByTag_Implementation(const FGameplayTag& MontageTag)
{
	for (FTagMontage Tag : AttackMontages) {
		if (Tag.MontageTag == MontageTag) {
			return Tag;
		}
	}
	return FTagMontage();
}

int AAuraCharacterBase::GetMinionCount_Implementation()
{
	return MinionCount;
}

void AAuraCharacterBase::IncreaseMinionCount_Implementation(int NumOfMinions)
{
	MinionCount += NumOfMinions;
}

void AAuraCharacterBase::MulticastHandleDeath_Implementation()
{
	UGameplayStatics::PlaySoundAtLocation(this, DeathSound, GetActorLocation(), GetActorRotation());
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

	OnDeath.Broadcast(this);
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

ECharacterClass AAuraCharacterBase::GetCharacterClass_Implementation()
{
	return CharacterClass;
}
