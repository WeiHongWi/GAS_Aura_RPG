// Fill out your copyright notice in the Description page of Project Settings.


#include "Passive/PassiveNiagaraComponent.h"

#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "Interaction/CombatInterface.h"

UPassiveNiagaraComponent::UPassiveNiagaraComponent()
{
	bAutoActivate = false;
}

void UPassiveNiagaraComponent::BeginPlay()
{
	Super::BeginPlay();

	//Bind the callback to the delegate from ASC comp.
	if (UAuraAbilitySystemComponent* AuraASC =
		Cast<UAuraAbilitySystemComponent>(
			UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetOwner())
		)) 
	{
		AuraASC->ActivatePassiveEffect.AddUObject(this,&UPassiveNiagaraComponent::OnPassiveActivate);
	}
	else if (ICombatInterface* CombatInterface = Cast<ICombatInterface>(GetOwner())) {
		CombatInterface->GetOnASCRegisterDelegate().AddLambda([this](UAbilitySystemComponent* ASC) {
			if (UAuraAbilitySystemComponent* AuraASC =
				Cast<UAuraAbilitySystemComponent>(ASC))
			{
				AuraASC->ActivatePassiveEffect.AddUObject(this, &UPassiveNiagaraComponent::OnPassiveActivate);
			}
		});
	}
}

void UPassiveNiagaraComponent::OnPassiveActivate(const FGameplayTag& AbilityTag, bool bActivate)
{
	if (AbilityTag.MatchesTagExact(NiagaraEffectTag)) {
		if (bActivate && !IsActive()) {
			Activate();
		}
		else {
			Deactivate();
		}
	}
}
