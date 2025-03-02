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
class UBehaviorTree;
class AAuraAIController;

UCLASS()
class AURA_API AAuraEnemy : public AAuraCharacter,public IEnemyInterface
{
	GENERATED_BODY()
	
public:
	AAuraEnemy();
	virtual void PossessedBy(AController* NewController) override;

	virtual void HighLight() override;
	virtual void UnHighLight() override;

	virtual AActor* GetCombatTarget_Implementation() override;

	virtual void SetCombatTarget_Implementation(AActor* CombatTarget) override;

	virtual int32 GetPlayerLevel_Implementation() override;

	UPROPERTY(BlueprintAssignable)
	FOnAttributeChangeSignature OnHealthChange;

	UPROPERTY(BlueprintAssignable)
	FOnAttributeChangeSignature OnMaxHealthChange;

	void HitReactTagChanged(const FGameplayTag GameplayTag,int32 NewCount);

	bool bHitReact = false;

	float BaseWalkSpeed = 350.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat")
	float LifeSpan = 5.f;

	UPROPERTY(BlueprintReadWrite, Category = "Combat")
	TObjectPtr<AActor> CombatTarget;
protected:
	virtual void BeginPlay() override;

	virtual void InitActorInfo() override;

	virtual void InitDefaultAttributes() override;

	virtual void Die(const FVector Impulse) override;


	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character Class Default")
	int32 level;

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly)
	TObjectPtr<UWidgetComponent> HealthBar;

	UPROPERTY(EditAnywhere, Category = "AI")
	TObjectPtr<UBehaviorTree> BehaviorTree;

	UPROPERTY()
	TObjectPtr<AAuraAIController> EnemyAIController;

};
