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

UCLASS()
class AURA_API AAuraPlayerState : public APlayerState,public IAbilitySystemInterface
{
	GENERATED_BODY()
	
public:
	AAuraPlayerState();

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<ULevelupInfo> LevelupInfo;

	FPlayerStateChangeSignature ExpChange;
	FPlayerStateChangeSignature LevelChange;

	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	UAttributeSet* GetAttributeSet() const { return AttributeSet; }

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const;

	FORCEINLINE int32 GetPlayerLevel() const { return level; }
	FORCEINLINE int32 GetXP() const { return exp; };

	void SetLevel(int32 LEVEL);
	void AddLevel(int32 LEVEL);
	void SetXP(int32 EXP);
	void AddXP(int32 EXP);

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
};
