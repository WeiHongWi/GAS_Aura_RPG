// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/AuraAbilitySystemLibrary.h"

#include "Kismet/GameplayStatics.h"
#include "UI/HUD/AuraHUD.h"
#include "Player/AuraPlayerState.h"
#include "UI/WidgetController/AuraWidgetController.h"
#include "Game/AuraGameModeBase.h"
#include "AuraAbilityTypes.h"
#include "Interaction/CombatInterface.h"
#include "AbilitySystem/Data/AbilityInfo.h"

bool UAuraAbilitySystemLibrary::MakeWidgetParams(const UObject* WorldContextObject, FWidgetControllerParams& WCParams, AAuraHUD*& AuraHUD)
{
	if (APlayerController* PC = UGameplayStatics::GetPlayerController(WorldContextObject, 0)) {
		AuraHUD = Cast<AAuraHUD>(PC->GetHUD());
		if (AuraHUD) {
			AAuraPlayerState* PS = PC->GetPlayerState<AAuraPlayerState>();
			UAbilitySystemComponent* ASC = PS->GetAbilitySystemComponent();
			UAttributeSet* AS = PS->GetAttributeSet();

			WCParams.AbilitySystemComponent = ASC;
			WCParams.AttributeSet = AS;
			WCParams.PlayerController = PC;
			WCParams.PlayerState = PS;

			return true;
		}
	}
	return false;
}

UOverlayAuraWidgetController* UAuraAbilitySystemLibrary::GetOverlayWidgetController(const UObject* WorldContextObject)
{
	FWidgetControllerParams WCParams;
	AAuraHUD* AuraHUD = nullptr;
	
	const bool bSetSuccessful = MakeWidgetParams(WorldContextObject, WCParams, AuraHUD);
	if (bSetSuccessful) {
		return AuraHUD->GetOverlayWidgetController(WCParams);
	}

	return nullptr;
}

UAttributeWidgetController* UAuraAbilitySystemLibrary::GetAttributeWidgetController(const UObject* WorldContextObject)
{
	FWidgetControllerParams WCParams;
	AAuraHUD* AuraHUD = nullptr;

	const bool bSetSuccessful = MakeWidgetParams(WorldContextObject, WCParams, AuraHUD);
	if (bSetSuccessful) {
		return AuraHUD->GetAttributeMenuWidgetController(WCParams);
	}
	return nullptr;
}

USpellMenuWidgetController* UAuraAbilitySystemLibrary::GetSpellMenuWidgetController(const UObject* WorldContextObject)
{
	FWidgetControllerParams WCParams;
	AAuraHUD* AuraHUD = nullptr;

	const bool bSetSuccessful = MakeWidgetParams(WorldContextObject, WCParams, AuraHUD);
	if (bSetSuccessful) {
		return AuraHUD->GetSpellMenuWidgetController(WCParams);
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
		if (ASC->GetAvatarActor()->Implements<UCombatInterface>()) {
			FGameplayAbilitySpec AbilitySpec = FGameplayAbilitySpec(StartupAbility, ICombatInterface::Execute_GetPlayerLevel(ASC->GetAvatarActor()));
			ASC->GiveAbility(AbilitySpec);
		}
	}
}

int32 UAuraAbilitySystemLibrary::GetRewardExpForClassAndLevel(const UObject* WorldContextObeject, ECharacterClass CharacterClass, int32 level)
{
	UCharacterClassInfo* CharacterClassInfo = GetCharacterClassInfo(WorldContextObeject);
	if (CharacterClassInfo == nullptr) return 0;

	//FScalableFloat curve  = CharacterClassInfo->CharacterInfo[CharacterClass].reward;

	FCharacterDefaultInfo Info = CharacterClassInfo->GetCharacterDefaultInfo(CharacterClass);
	FScalableFloat curve = Info.reward;
	float exp = curve.GetValueAtLevel(level);

	return static_cast<int32>(exp);
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

UAbilityInfo* UAuraAbilitySystemLibrary::GetAbilityInfo(const UObject* WorldContextObeject)
{
	AAuraGameModeBase* GameMode = Cast<AAuraGameModeBase>(UGameplayStatics::GetGameMode(WorldContextObeject));
	if (!GameMode) {
		return nullptr;
	}

	UAbilityInfo* AbilityInfo = GameMode->AuraAbilityInfo;
	return AbilityInfo;
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

