// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/AuraAbilitySystemLibrary.h"

#include "Kismet/GameplayStatics.h"
#include "UI/HUD/AuraHUD.h"
#include "Player/AuraPlayerState.h"
#include "UI/WidgetController/AuraWidgetController.h"
#include "Game/AuraGameModeBase.h"
#include "AuraAbilityTypes.h"
#include "Interaction/CombatInterface.h"

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

void UAuraAbilitySystemLibrary::InitializeDefualtGameplayAbility(const UObject* WorldContextObeject, UAbilitySystemComponent* ASC, ECharacterClass Class)
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

	const FCharacterDefaultInfo DefaultInfo = CharacterInfo->GetCharacterDefaultInfo(Class);
	
	for (auto& StartupAbility : DefaultInfo.StartUpAbilities) {
		if (ICombatInterface* CI = Cast<ICombatInterface>(ASC->GetAvatarActor())) {
			FGameplayAbilitySpec AbilitySpec = FGameplayAbilitySpec(StartupAbility, CI->GetPlayerLevel());
			ASC->GiveAbility(AbilitySpec);
		}
	}
	
}

UCharacterClassInfo* UAuraAbilitySystemLibrary::GetCharacterClassInfo(const UObject* WorldContextObeject)
{
	AAuraGameModeBase* GameMode = Cast<AAuraGameModeBase>(UGameplayStatics::GetGameMode(WorldContextObeject));
	if (!GameMode) {
		return nullptr;
	}

	UCharacterClassInfo* CharacterInfo = GameMode->CharacterClassInfo;
	return CharacterInfo;
}

bool UAuraAbilitySystemLibrary::IsBlockHit(const FGameplayEffectContextHandle& EffectContextHandle)
{
	if (const FAuraGameplayEffectContext* AuraEffectContextHandle 
		= static_cast<const FAuraGameplayEffectContext*>(EffectContextHandle.Get())) {
	
		return AuraEffectContextHandle->IsBlockedHit();
	}
	return false;
}

bool UAuraAbilitySystemLibrary::IsCriticalHit(const FGameplayEffectContextHandle& EffectContextHandle)
{
	if (const FAuraGameplayEffectContext* AuraEffectContextHandle
		= static_cast<const FAuraGameplayEffectContext*>(EffectContextHandle.Get())) {

		return AuraEffectContextHandle->IsCriticalHit();
	}
	return false;
}

void UAuraAbilitySystemLibrary::SetIsBlockHit(UPARAM(ref)FGameplayEffectContextHandle& EffectContextHandle, bool bInIsBlockHit)
{
	if (FAuraGameplayEffectContext* AuraEffectContextHandle = 
		static_cast<FAuraGameplayEffectContext*>(EffectContextHandle.Get())) {

		AuraEffectContextHandle->SetIsBlockedHit(bInIsBlockHit);
	}
	
}

void UAuraAbilitySystemLibrary::SetIsCriticalHit(UPARAM(ref)FGameplayEffectContextHandle& EffectContextHandle, bool bInIsCriticalHit)
{
	if (FAuraGameplayEffectContext* AuraEffectContextHandle =
		static_cast<FAuraGameplayEffectContext*>(EffectContextHandle.Get())) {

		AuraEffectContextHandle->SetIsCriticalHit(bInIsCriticalHit);
	}
}

void UAuraAbilitySystemLibrary::GetLivePlayersWithinRadius(const UObject* WorldContextObject, TArray<AActor*>& OutOverlappingActors, const TArray<AActor*>& ActorsToIgnore, float Radius, const FVector& SphereOrigin)
{
	FCollisionQueryParams SphereParams;
	SphereParams.AddIgnoredActors(ActorsToIgnore);

	// query scene to see what we hit
	TArray<FOverlapResult> Overlaps;
	if (UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull))
	{
		World->OverlapMultiByObjectType(
			Overlaps, 
			SphereOrigin, 
			FQuat::Identity,
			FCollisionObjectQueryParams(FCollisionObjectQueryParams::InitType::AllDynamicObjects), FCollisionShape::MakeSphere(Radius), SphereParams
		);
		for (auto& Overlap : Overlaps) {
			//Is this actor die?
			//CombatInterface exist?
			if (Overlap.GetActor()->Implements<UCombatInterface>() && !ICombatInterface::Execute_bIsDead(Overlap.GetActor())) {
				OutOverlappingActors.AddUnique(ICombatInterface::Execute_GetAvatarActor(Overlap.GetActor()));
			}
		}
	}

}

bool UAuraAbilitySystemLibrary::IsFriend(AActor* FirstActor, AActor* SecondActor)
{
	const bool PlayerFriend = FirstActor->ActorHasTag("Player") && SecondActor->ActorHasTag("Player");
	const bool EnemyFriend = FirstActor->ActorHasTag("Enemy") && SecondActor->ActorHasTag("Enemy");

	return PlayerFriend || EnemyFriend;
}
