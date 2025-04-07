// Fill out your copyright notice in the Description page of Project Settings.


#include "Passive/PassiveNiagaraComponent.h"

#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "Interaction/CombatInterface.h"
#include "AuraGameplayTags.h"

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
		const bool bIsStartupAbilityInitialized = AuraASC->bIsStartupAbilityInitialized;
		if (bIsStartupAbilityInitialized) {
			if (AuraASC->GetStatusByAbilityTag(NiagaraEffectTag).MatchesTagExact(FAuraGameplayTags::Get().Abilities_Status_Equipped)) {
				Activate();
			}
		}
	}
	else if (ICombatInterface* CombatInterface = Cast<ICombatInterface>(GetOwner())) {
		CombatInterface->GetOnASCRegisterDelegate().AddLambda([this](UAbilitySystemComponent* ASC) {
			if (UAuraAbilitySystemComponent* AuraASC =
				Cast<UAuraAbilitySystemComponent>(ASC))
			{
				AuraASC->ActivatePassiveEffect.AddUObject(this, &UPassiveNiagaraComponent::OnPassiveActivate);
				const bool bIsStartupAbilityInitialized = AuraASC->bIsStartupAbilityInitialized;
				if (bIsStartupAbilityInitialized) {
					if (AuraASC->GetStatusByAbilityTag(NiagaraEffectTag).MatchesTagExact(FAuraGameplayTags::Get().Abilities_Status_Equipped)) {
						Activate();
					}
				}
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
