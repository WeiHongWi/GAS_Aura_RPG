// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/AuraCharacter.h"
#include "Interaction/EnemyInterface.h"
#include "AbilitySystemInterface.h"
#include "UI/WidgetController/OverlayAuraWidgetController.h"
#include "AbilitySystem/Data/CharacterClassInfo.h"
#include "AuraEnemy.generated.h"

/**
 * 
 */
class UWidgetComponent;

UCLASS()
class AURA_API AAuraEnemy : public AAuraCharacter,public IEnemyInterface
{
	GENERATED_BODY()
	
public:
	AAuraEnemy();

	virtual void HighLight() override;
	virtual void UnHighLight() override;

	virtual int32 GetPlayerLevel() override;

	UPROPERTY(BlueprintAssignable)
	FOnAttributeChangeSignature OnHealthChange;

	UPROPERTY(BlueprintAssignable)
	FOnAttributeChangeSignature OnMaxHealthChange;

	void HitReactTagChanged(const FGameplayTag GameplayTag,int32 NewCount);

	bool bHitReact = false;

	float BaseWalkSpeed = 250.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat")
	float LifeSpan = 5.f;
protected:
	virtual void BeginPlay() override;

	virtual void InitActorInfo() override;

	virtual void InitDefaultAttributes() override;

	virtual void Die() override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character Class Default")
	int32 level;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character Class Default")
	ECharacterClass CharacterClass;

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly)
	TObjectPtr<UWidgetComponent> HealthBar;

};
