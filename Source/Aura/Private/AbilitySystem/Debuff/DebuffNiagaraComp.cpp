// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Debuff/DebuffNiagaraComp.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "Interaction/CombatInterface.h"

UDebuffNiagaraComp::UDebuffNiagaraComp()
{
	bAutoActivate = false;
}

void UDebuffNiagaraComp::BeginPlay()
{
	Super::BeginPlay();

	ICombatInterface* CombatInterface = Cast<ICombatInterface>(GetOwner());
	UAbilitySystemComponent* ASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetOwner());
	if (ASC) {
		ASC->RegisterGameplayTagEvent(DebuffTag, EGameplayTagEventType::NewOrRemoved).AddUObject(this, &UDebuffNiagaraComp::DebuffTagChanged);
	}
	
	else if (CombatInterface) {
		CombatInterface->GetOnASCRegisterDelegate().AddWeakLambda(this, [this](UAbilitySystemComponent* InASC) {
			InASC->RegisterGameplayTagEvent(DebuffTag, EGameplayTagEventType::NewOrRemoved).AddUObject(this, &UDebuffNiagaraComp::DebuffTagChanged);
			});
	}
	if (CombatInterface) {
		CombatInterface->GetOnDeathDelegate().AddDynamic(this, &UDebuffNiagaraComp::DebuffDeactivate);
	}
}

void UDebuffNiagaraComp::DebuffTagChanged(const FGameplayTag  CallbackTag, int32 NewCount)
{
	const bool bOwnerValid = IsValid(GetOwner());
	const bool bOwnerAlive = GetOwner()->Implements<UCombatInterface>() && !ICombatInterface::Execute_bIsDead(GetOwner());
	
	if (NewCount > 0 && bOwnerValid && bOwnerAlive) {
		Activate();
	}
	else {
		Deactivate();
	}
}

void UDebuffNiagaraComp::DebuffDeactivate(AActor* DeathActor)
{
	Deactivate();
}
