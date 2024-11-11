// Fill out your copyright notice in the Description page of Project Settings.


#include "Actor/AuraEffectActor.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemInterface.h"
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

	FGameplayEffectSpecHandle EffectSpecHandle =  ASC->MakeOutgoingSpec(GEffectClass, 1.0f, EffectHandle);
	ASC->ApplyGameplayEffectSpecToSelf(*EffectSpecHandle.Data.Get());
	

}

void AAuraEffectActor::OnOverlap(AActor* TargetActor)
{
}

void AAuraEffectActor::EndOverlap(AActor* TargetActor)
{
}

