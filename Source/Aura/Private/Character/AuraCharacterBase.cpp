// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/AuraCharacterBase.h"
#include "AbilitySystemComponent.h"

// Sets default values
AAuraCharacterBase::AAuraCharacterBase()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

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
}



// Called when the game starts or when spawned
void AAuraCharacterBase::BeginPlay()
{
	Super::BeginPlay();
	
}


