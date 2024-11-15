// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/AuraAttributeSet.h"

#include "GameplayEffectExtension.h"
#include "GameFramework/Character.h"
#include "AbilitySystemBlueprintLibrary.h"
#include <Net/UnrealNetwork.h>
#include "Aura/Aura.h"

UAuraAttributeSet::UAuraAttributeSet()
{
	InitHealth(INIT_PLAYER_HEALTH);
	InitMaxHealth(100.f);
	InitMana(INIT_PLAYER_MANA);
	InitMaxMana(100.f);
}

void UAuraAttributeSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet,Health,COND_None,REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, MaxHealth, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, Mana, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, MaxMana, COND_None, REPNOTIFY_Always);
	
}

void UAuraAttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeBaseChange(Attribute, NewValue);

	if (Attribute == GetHealthAttribute()) {
		NewValue = FMath::Clamp(NewValue, 0.f, GetMaxHealth());
	}
	if (Attribute == GetManaAttribute()) {
		NewValue = FMath::Clamp(NewValue, 0.f, GetMaxMana());
	}
}
void UAuraAttributeSet::SetEffectProperties(const FGameplayEffectModCallbackData& data, FEffectProperties& Props) const
{
	// ~Begin source
	Props.EffectContextHandle = data.EffectSpec.GetContext();
	Props.ASCSource = Props.EffectContextHandle.GetInstigatorAbilitySystemComponent();

	if (IsValid(Props.ASCSource) && Props.ASCSource->AbilityActorInfo.IsValid()
		&& Props.ASCSource->AbilityActorInfo->AvatarActor.IsValid()) {
		Props.SourceAvatarActor = Props.ASCSource->AbilityActorInfo->AvatarActor.Get();
		Props.SourceController = Props.ASCSource->AbilityActorInfo->PlayerController.Get();

		if (Props.SourceController == nullptr && Props.SourceAvatarActor != nullptr) {
			if (const APawn* Pawn = Cast<APawn>(Props.SourceAvatarActor)) {
				Props.SourceController = Pawn->GetController();
			}
		}
		if (Props.SourceController) {
			Props.SourceCharacter = Cast<ACharacter>(Props.SourceController->GetPawn());
		}

	}
	//~End Source

	//~Begin Target
	if (data.Target.AbilityActorInfo.IsValid() &&
		data.Target.AbilityActorInfo->AvatarActor.IsValid()) {

		Props.TargetAvatarActor = data.Target.AbilityActorInfo->AvatarActor.Get();
		Props.TargetController = data.Target.AbilityActorInfo->PlayerController.Get();
		Props.TargetCharacter = Cast<ACharacter>(Props.TargetAvatarActor);
		Props.ASCTarget = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(Props.TargetAvatarActor);
	}

	//~End Target
}
void UAuraAttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& data)
{
	//Source: causer of effect, Target: target of effect
	Super::PostGameplayEffectExecute(data);
	
	struct FEffectProperties EffectProperties;
	SetEffectProperties(data, EffectProperties);
	
}



void UAuraAttributeSet::OnRep_Health(const struct FGameplayAttributeData& OldHealth) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, Health, OldHealth);
}

void UAuraAttributeSet::OnRep_MaxHealth(const struct FGameplayAttributeData& OldMaxHealth) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, MaxHealth, OldMaxHealth);
}

void UAuraAttributeSet::OnRep_Mana(const FGameplayAttributeData& OldMana) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, Mana, OldMana);
}

void UAuraAttributeSet::OnRep_MaxMana(const FGameplayAttributeData& OldMaxMana) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, MaxMana, OldMaxMana);
}



