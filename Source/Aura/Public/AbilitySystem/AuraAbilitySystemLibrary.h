// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystem/Data/CharacterClassInfo.h"
#include "AuraAbilitySystemLibrary.generated.h"


class UOverlayAuraWidgetController;
class UAttributeWidgetController;
class USpellMenuWidgetController;
struct FWidgetControllerParams;
/**
 * 
 */
UCLASS()
class AURA_API UAuraAbilitySystemLibrary : public UAbilitySystemBlueprintLibrary
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintPure, Category = "AuraAbilitySystemLibrary|WidgetController")
	static bool  MakeWidgetParams(const UObject* WorldContextObject, FWidgetControllerParams& WCParams,AAuraHUD*& AuraHUD);

	UFUNCTION(BlueprintPure, Category = "AuraAbilitySystemLibrary|WidgetController")
	static UOverlayAuraWidgetController* GetOverlayWidgetController(const UObject* WorldContextObject);

	UFUNCTION(BlueprintPure, Category = "AuraAbilitySystemLibrary|WidgetController")
	static UAttributeWidgetController* GetAttributeWidgetController(const UObject* WorldContextObject);

	UFUNCTION(BlueprintPure, Category = "AuraAbilitySystemLibrary|WidgetController")
	static USpellMenuWidgetController* GetSpellMenuWidgetController(const UObject* WorldContextObject);
	
	UFUNCTION(BlueprintCallable, Category = "AuraAbilitySystemLibrary|CharacterDefaultInfo")
	static void InitializeDefaultAttribute(const UObject* WorldContextObject,ECharacterClass Class, float level,UAbilitySystemComponent* ASC);

	UFUNCTION(BlueprintCallable, Category = "AuraAbilitySystemLibrary|AbilityDefaultInfo")
	static void InitializeDefualtGameplayAbility(const UObject* WorldContextObeject, UAbilitySystemComponent* ASC, ECharacterClass Class);

	UFUNCTION(BlueprintCallable, Category = "AuraAbilitySystemLibrary|AbilityDefaultInfo")
	static UCharacterClassInfo* GetCharacterClassInfo(const UObject* WorldContextObeject);
	
	UFUNCTION(BlueprintCallable, Category = "AuraAbilitySystemLibrary|AbilityDefaultInfo")
	static UAbilityInfo* GetAbilityInfo(const UObject* WorldContextObeject);

	UFUNCTION(BlueprintPure, Category = "AuraAbilitySystemLibrary|EffectContext")
	static bool IsBlockHit(const FGameplayEffectContextHandle& EffectContextHandle);

	UFUNCTION(BlueprintPure, Category = "AuraAbilitySystemLibrary|EffectContext")
	static bool IsCriticalHit(const FGameplayEffectContextHandle& EffectContextHandle);

	UFUNCTION(BlueprintCallable, Category = "AuraAbilitySystemLibrary|EffectContext")
	static void SetIsBlockHit(UPARAM(ref) FGameplayEffectContextHandle& EffectContextHandle,bool bInIsBlockHit);

	UFUNCTION(BlueprintCallable, Category = "AuraAbilitySystemLibrary|EffectContext")
	static void SetIsCriticalHit(UPARAM(ref) FGameplayEffectContextHandle& EffectContextHandle, bool bInIsCriticalHit);

	UFUNCTION(BlueprintCallable, Category = "AuraAbilitySystemLibrary|GameaplayMechanics")
	static void GetLivePlayersWithinRadius(
		const UObject* WorldContextObject,
		TArray<AActor*>& OutOverlappingActors, 
		const TArray<AActor*>& ActorsToIgnore, 
		float Radius, 
		const FVector& SphereOrigin);

	UFUNCTION(BlueprintPure,Category = "AuraAbilitySystemLibrary|GameaplayMechanics")
	static bool IsFriend(AActor* FirstActor, AActor* SecondActor);

	static int32 GetRewardExpForClassAndLevel(const UObject* WorldContextObeject, ECharacterClass CharacterClass, int32 level);
};
