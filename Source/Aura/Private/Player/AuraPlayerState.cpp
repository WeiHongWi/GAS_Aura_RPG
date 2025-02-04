// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/AuraPlayerState.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "AbilitySystem/AuraAttributeSet.h"
#include "Net/UnrealNetwork.h"

AAuraPlayerState::AAuraPlayerState()
{
	AbilitySystemComp = CreateDefaultSubobject<UAuraAbilitySystemComponent>("AbilitySystemComp");
	AbilitySystemComp->SetIsReplicated(true);
	AbilitySystemComp->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);

	AttributeSet = CreateDefaultSubobject<UAuraAttributeSet>("AttributeSet");
	NetUpdateFrequency = 100.f;
}

UAbilitySystemComponent* AAuraPlayerState::GetAbilitySystemComponent() const
{
	return AbilitySystemComp;
}

void AAuraPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AAuraPlayerState, level);
	DOREPLIFETIME(AAuraPlayerState, exp);
}

void AAuraPlayerState::SetLevel(int32 LEVEL)
{
	level = LEVEL;
	LevelChange.Broadcast(level);
}

void AAuraPlayerState::AddLevel(int32  LEVEL)
{
	level += LEVEL;
	LevelChange.Broadcast(level);
}

void AAuraPlayerState::SetXP(int32 EXP)
{
	exp = EXP;
	ExpChange.Broadcast(exp);
}

void AAuraPlayerState::AddXP(int32 EXP)
{
	exp += EXP;
	ExpChange.Broadcast(exp);
}


void AAuraPlayerState::OnRep_Level(int32 OldLevel)
{
	LevelChange.Broadcast(level);
}

void AAuraPlayerState::OnRep_Exp(int32 OldExp)
{
	ExpChange.Broadcast(exp);
}
