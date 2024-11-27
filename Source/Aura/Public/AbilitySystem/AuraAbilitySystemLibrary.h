// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystem/Data/CharacterClassInfo.h"
#include "AuraAbilitySystemLibrary.generated.h"


class UOverlayAuraWidgetController;
class UAttributeWidgetController;
/**
 * 
 */
UCLASS()
class AURA_API UAuraAbilitySystemLibrary : public UAbilitySystemBlueprintLibrary
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintPure, Category = "AuraAbilitySystemLibrary|WidgetController")
	static UOverlayAuraWidgetController* GetOverlayWidgetController(const UObject* WorldContextObject);

	UFUNCTION(BlueprintPure, Category = "AuraAbilitySystemLibrary|WidgetController")
	static UAttributeWidgetController* GetAttributeWidgetController(const UObject* WorldContextObject);

	UFUNCTION(BlueprintCallable, Category = "AuraAbilitySystemLibrary|CharacterDefaultInfo")
	static void InitializeDefaultAttribute(const UObject* WorldContextObject,ECharacterClass Class, float level,UAbilitySystemComponent* ASC);

	UFUNCTION(BlueprintCallable, Category = "AuraAbilitySystemLibrary|AbilityDefaultInfo")
	static void InitializeDefualtGameplayAbility(const UObject* WorldContextObeject, UAbilitySystemComponent* ASC);

};
