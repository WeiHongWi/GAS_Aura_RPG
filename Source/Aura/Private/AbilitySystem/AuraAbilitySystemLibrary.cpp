// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/AuraAbilitySystemLibrary.h"

#include "Kismet/GameplayStatics.h"
#include "UI/HUD/AuraHUD.h"
#include "Player/AuraPlayerState.h"
#include "UI/WidgetController/AuraWidgetController.h"
#include "Game/AuraGameModeBase.h"

UOverlayAuraWidgetController* UAuraAbilitySystemLibrary::GetOverlayWidgetController(const UObject* WorldContextObject)
{
	if (APlayerController* PC = UGameplayStatics::GetPlayerController(WorldContextObject, 0)) {
		if (AAuraHUD* HUD = Cast<AAuraHUD>(PC->GetHUD())) {
			AAuraPlayerState* PS = PC->GetPlayerState<AAuraPlayerState>();

			UAbilitySystemComponent* ASC = PS->GetAbilitySystemComponent();
			UAttributeSet* AS = PS->GetAttributeSet();

			const FWidgetControllerParams WidgetControllerParams(PC, PS, ASC, AS);
			return HUD->GetOverlayWidgetController(WidgetControllerParams);
		}
	}

	return nullptr;
}

UAttributeWidgetController* UAuraAbilitySystemLibrary::GetAttributeWidgetController(const UObject* WorldContextObject)
{
	if (APlayerController* PC = UGameplayStatics::GetPlayerController(WorldContextObject, 0)) {
		if (AAuraHUD* HUD = Cast<AAuraHUD>(PC->GetHUD())) {
			AAuraPlayerState* PS = PC->GetPlayerState<AAuraPlayerState>();

			UAbilitySystemComponent* ASC = PS->GetAbilitySystemComponent();
			UAttributeSet* AS = PS->GetAttributeSet();

			const FWidgetControllerParams WidgetControllerParams(PC, PS, ASC, AS);
			return HUD->GetAttributeMenuWidgetController(WidgetControllerParams);
		}
	}

	return nullptr;
}

void UAuraAbilitySystemLibrary::InitializeDefaultAttribute(const UObject* WorldContextObject,ECharacterClass Class, float level,UAbilitySystemComponent* ASC)
{
	AAuraGameModeBase* GameMode = Cast<AAuraGameModeBase>(UGameplayStatics::GetGameMode(WorldContextObject));
	if (!GameMode) {
		return;
	}
	AActor* AvatarActor = ASC->GetAvatarActor();

	UCharacterClassInfo* CharacterClassInfo = GameMode->CharacterClassInfo;
	FCharacterDefaultInfo CharacterInfo = CharacterClassInfo->GetCharacterDefaultInfo(Class);

	FGameplayEffectContextHandle PrimaryContextHandle = ASC->MakeEffectContext();
	PrimaryContextHandle.AddSourceObject(AvatarActor);
	FGameplayEffectSpecHandle EffectSpecHandle = ASC->MakeOutgoingSpec(CharacterInfo.PrimaryAttributes, level, PrimaryContextHandle);
	ASC->ApplyGameplayEffectSpecToSelf(*EffectSpecHandle.Data.Get());

	FGameplayEffectContextHandle SecondaryContextHandle = ASC->MakeEffectContext();
	SecondaryContextHandle.AddSourceObject(AvatarActor);
	FGameplayEffectSpecHandle SecondaryEffectSpecHandle = ASC->MakeOutgoingSpec(CharacterClassInfo->SecondaryAttributes, level, SecondaryContextHandle);
	ASC->ApplyGameplayEffectSpecToSelf(*SecondaryEffectSpecHandle.Data.Get());

	FGameplayEffectContextHandle VitalContextHandle = ASC->MakeEffectContext();
	VitalContextHandle.AddSourceObject(AvatarActor);
	FGameplayEffectSpecHandle VitalEffectSpecHandle = ASC->MakeOutgoingSpec(CharacterClassInfo->VitalAttributes, level, VitalContextHandle);
	ASC->ApplyGameplayEffectSpecToSelf(*VitalEffectSpecHandle.Data.Get());
}

void UAuraAbilitySystemLibrary::InitializeDefualtGameplayAbility(const UObject* WorldContextObeject, UAbilitySystemComponent* ASC)
{
	AAuraGameModeBase* GameMode = Cast<AAuraGameModeBase>(UGameplayStatics::GetGameMode(WorldContextObeject));
	if (!GameMode) {
		return;
	}

	UCharacterClassInfo* CharacterInfo = GameMode->CharacterClassInfo;
	for (auto& AbilityClass : CharacterInfo->CommonAbilities) {
		FGameplayAbilitySpec AbilitySpec = FGameplayAbilitySpec(AbilityClass, 1);
		ASC->GiveAbility(AbilitySpec);
	}
}
