// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/AuraCharacter.h"
#include "Interaction/EnemyInterface.h"
#include "AbilitySystemInterface.h"
#include "AuraEnemy.generated.h"

/**
 * 
 */
UCLASS()
class AURA_API AAuraEnemy : public AAuraCharacter,public IEnemyInterface
{
	GENERATED_BODY()
	
public:
	AAuraEnemy();

	virtual void HighLight() override;
	virtual void UnHighLight() override;


protected:
	virtual void BeginPlay() override;

	virtual void InitActorInfo() override;
};
