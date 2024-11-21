// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "GameplayTagContainer.h"
#include "InputActionDataAsset.generated.h"

class UInputAction;

USTRUCT(BlueprintType)
struct FAuraInputAction {

	GENERATED_BODY();

	UPROPERTY(EditDefaultsOnly)
	const class UInputAction* AbilityInputAction = nullptr;

	UPROPERTY(EditDefaultsOnly)
	FGameplayTag GameplayTag = FGameplayTag();
};

/**
 * 
 */
UCLASS()
class AURA_API UInputActionDataAsset : public UDataAsset
{
	GENERATED_BODY()

public:
	const UInputAction* FindAbilityInputActionInfo(const FGameplayTag& GameplayTag, bool bLogFound);

	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly)
	TArray<FAuraInputAction> AbilityInputAction;
	
};
