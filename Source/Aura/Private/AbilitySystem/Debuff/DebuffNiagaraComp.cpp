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
		ASC->RegisterGameplayTagEvent(DebuffTag, EGameplayTagEventType::NewOrRemoved).AddUObject(this, &UDebuffNiagaraComp::DebuffChanged);
	}
	else if (CombatInterface) {
		CombatInterface->GetOnASCRegisterDelegate().AddWeakLambda(this, [this](UAbilitySystemComponent* InASC) {
			InASC->RegisterGameplayTagEvent(DebuffTag, EGameplayTagEventType::NewOrRemoved).AddUObject(this, &UDebuffNiagaraComp::DebuffChanged);
			});
	}
	if (CombatInterface) {
		CombatInterface->GetOnDeathDelegate().AddDynamic(this, &UDebuffNiagaraComp::DebuffDeactivate);
	}
}

void UDebuffNiagaraComp::DebuffChanged(const FGameplayTag Tag, int32 NewCount)
{
	if (NewCount > 0) {
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
