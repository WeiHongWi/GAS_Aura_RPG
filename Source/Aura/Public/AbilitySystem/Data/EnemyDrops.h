// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "EnemyDrops.generated.h"



/**
 * 
 */
UCLASS()
class AURA_API UEnemyDrops : public UDataAsset
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintReadOnly,EditDefaultsOnly, Category = "EnemyDrops")
	TArray<TSubclassOf<AActor>>Drops;
};
