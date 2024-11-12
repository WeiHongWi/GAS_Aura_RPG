// Fill out your copyright notice in the Description page of Project Settings.


#include "Actor/AuraEffectActor.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemInterface.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystem/AuraAttributeSet.h"


AAuraEffectActor::AAuraEffectActor()
{
	PrimaryActorTick.bCanEverTick = false;

	SetRootComponent(CreateDefaultSubobject<USceneComponent>("SceneRoot"));
}

void AAuraEffectActor::BeginPlay()
{
	Super::BeginPlay();
	
}

void AAuraEffectActor::ApplyEffectOnTarget(AActor* Actor, TSubclassOf<UGameplayEffect> GEffectClass)
{
	IAbilitySystemInterface* ASCInterface = Cast<IAbilitySystemInterface>(Actor);
	if (!ASCInterface) {
		return;
	}
	check(GEffectClass);
	UAbilitySystemComponent* ASC = ASCInterface->GetAbilitySystemComponent();

	FGameplayEffectContextHandle EffectHandle = ASC->MakeEffectContext();
	EffectHandle.AddSourceObject(this);

	FGameplayEffectSpecHandle EffectSpecHandle = ASC->MakeOutgoingSpec(GEffectClass, EffectLevel , EffectHandle);
	FActiveGameplayEffectHandle ActiveEffectHandle =  ASC->ApplyGameplayEffectSpecToSelf(*EffectSpecHandle.Data.Get());
	
	const bool bIsInfinite =
		EffectSpecHandle.Data.Get()->Def.Get()->DurationPolicy == EGameplayEffectDurationType::Infinite;

	if (bIsInfinite && InfiniteGameplayEffectRemovePolicy == EEffectRemovePolicy::RemoveEndOverlap) {
		ActiveEffectHandles.Add(ActiveEffectHandle, ASC);
	}
}

void AAuraEffectActor::OnOverlap(AActor* TargetActor)
{
	if (GameplayEffectPolicy == EEffectApplyPolicy::ApplyOnOverlap) {
		ApplyEffectOnTarget(TargetActor, GameplayEffectClass);
	}

	if (DurationGameplayEffectPolicy == EEffectApplyPolicy::ApplyOnOverlap) {
		ApplyEffectOnTarget(TargetActor, DurationGameplayEffectClass);
	}
	if (InfiniteGameplayEffectPolicy == EEffectApplyPolicy::ApplyOnOverlap) {
		ApplyEffectOnTarget(TargetActor, InfiniteGameplayEffectClass);
	}
}

void AAuraEffectActor::EndOverlap(AActor* TargetActor)
{
	if (GameplayEffectPolicy == EEffectApplyPolicy::ApplyEndOverlap) {
		ApplyEffectOnTarget(TargetActor, GameplayEffectClass);
	}

	if (DurationGameplayEffectPolicy == EEffectApplyPolicy::ApplyEndOverlap) {
		ApplyEffectOnTarget(TargetActor, DurationGameplayEffectClass);
	}

	if (InfiniteGameplayEffectPolicy == EEffectApplyPolicy::ApplyEndOverlap) {
		ApplyEffectOnTarget(TargetActor, InfiniteGameplayEffectClass);
	}

	if (InfiniteGameplayEffectRemovePolicy == EEffectRemovePolicy::RemoveEndOverlap) {
		UAbilitySystemComponent* ASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetActor);
		if (!IsValid(ASC)) { return; }

		TArray<FActiveGameplayEffectHandle> HandleToRemove;
		for (auto i : ActiveEffectHandles) {
			if (i.Value == ASC) {
				ASC->RemoveActiveGameplayEffect(i.Key,1);
				HandleToRemove.Add(i.Key);
			}
		}

		for (auto i : HandleToRemove) {
			ActiveEffectHandles.FindAndRemoveChecked(i);
		}
	
	}
}

