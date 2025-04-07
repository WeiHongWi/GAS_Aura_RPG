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
	DOREPLIFETIME(AAuraPlayerState, AttributePoints);
	DOREPLIFETIME(AAuraPlayerState, SpellPoints);
}

void AAuraPlayerState::SetLevel(int32 LEVEL)
{
	level = LEVEL;
	LevelChange.Broadcast(level,false);
}

void AAuraPlayerState::AddLevel(int32  LEVEL)
{
	level += LEVEL;
	LevelChange.Broadcast(level,true);
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

void AAuraPlayerState::SetCoin(int32 InCoin)
{
	Coin = InCoin;
	CoinChange.Broadcast(Coin);
}

void AAuraPlayerState::AddCoin(int32 InCoin)
{
	Coin += InCoin;
	CoinChange.Broadcast(Coin);
}


void AAuraPlayerState::AddAttributePoints(int32 Points)
{
	AttributePoints += Points;
	AttributePointsChange.Broadcast(AttributePoints);
}

void AAuraPlayerState::SetAttributePoints(int32 Points)
{
	AttributePoints = Points;
	AttributePointsChange.Broadcast(AttributePoints);
}

void AAuraPlayerState::AddSpellPoints(int32 Points)
{
	SpellPoints += Points;
	SpellPointsChange.Broadcast(SpellPoints);
}

void AAuraPlayerState::SetSpellPoints(int32 Points)
{
	SpellPoints = Points;
	SpellPointsChange.Broadcast(SpellPoints);
}


void AAuraPlayerState::OnRep_Level(int32 OldLevel)
{
	LevelChange.Broadcast(level,true);
}

void AAuraPlayerState::OnRep_Exp(int32 OldExp)
{
	ExpChange.Broadcast(exp);
}

void AAuraPlayerState::OnRep_Coin(int32 OldCoin)
{
	CoinChange.Broadcast(Coin);
}

void AAuraPlayerState::OnRep_AttributePoints(int32 OldAttributePoints)
{
	AttributePointsChange.Broadcast(AttributePoints);
}

void AAuraPlayerState::OnRep_SpellPoints(int32 OldSpellPoints)
{
	SpellPointsChange.Broadcast(SpellPoints);
}
