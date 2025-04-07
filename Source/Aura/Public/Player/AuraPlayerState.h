// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "AbilitySystemInterface.h"
#include "AuraPlayerState.generated.h"

/**
 * 
 */

class UAbilitySystemComponent;
class UAttributeSet;
class ULevelupInfo;

DECLARE_MULTICAST_DELEGATE_OneParam(FPlayerStateChangeSignature, int32/*State Change*/);
DECLARE_MULTICAST_DELEGATE_TwoParams(FOnLevelChanged, int32 /*StatValue*/, bool /*bLevelUp*/)


UCLASS()
class AURA_API AAuraPlayerState : public APlayerState,public IAbilitySystemInterface
{
	GENERATED_BODY()
	
public:
	AAuraPlayerState();

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<ULevelupInfo> LevelupInfo;

	// Player State Change Delegate
	FPlayerStateChangeSignature ExpChange;
	FOnLevelChanged LevelChange;
	FPlayerStateChangeSignature AttributePointsChange;
	FPlayerStateChangeSignature SpellPointsChange;

	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	UAttributeSet* GetAttributeSet() const { return AttributeSet; }

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const;

	FORCEINLINE int32 GetPlayerLevel() const { return level; }
	FORCEINLINE int32 GetXP() const { return exp; };
	FORCEINLINE int32 GetAttributePoints() const { return AttributePoints; }
	FORCEINLINE int32 GetSpellPoints() const { return SpellPoints; }

	void SetLevel(int32 LEVEL);
	void AddLevel(int32 LEVEL);
	void SetXP(int32 EXP);
	void AddXP(int32 EXP);

	void AddAttributePoints(int32 Points);
	void SetAttributePoints(int32 Points);

	void AddSpellPoints(int32 Points);
	void SetSpellPoints(int32 Points);

protected:
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UAbilitySystemComponent> AbilitySystemComp;

	UPROPERTY()
	TObjectPtr<UAttributeSet> AttributeSet;

	UPROPERTY(VisibleAnywhere, ReplicatedUsing = OnRep_Level)
	int32 level = 1.f;

	UFUNCTION()
	void OnRep_Level(int32 OldLevel);

	UPROPERTY(VisibleAnywhere, ReplicatedUsing = OnRep_Exp)
	int32 exp = 0.f;

	UFUNCTION()
	void OnRep_Exp(int32 OldExp);

	UPROPERTY(VisibleAnywhere, ReplicatedUsing = OnRep_AttributePoints)
	int32 AttributePoints = 0;

	UFUNCTION()
	void OnRep_AttributePoints(int32 OldAttributePoints);

	UPROPERTY(VisibleAnywhere, ReplicatedUsing = OnRep_SpellPoints)
	int32 SpellPoints = 0;

	UFUNCTION()
	void OnRep_SpellPoints(int32 OldSpellPoints);
};
