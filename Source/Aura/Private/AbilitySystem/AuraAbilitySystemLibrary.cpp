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
#include "AuraGameplayTags.h"

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

FGameplayEffectContextHandle UAuraAbilitySystemLibrary::ApplyDamageEffect(const FDamageEffectParams& Params)
{
	FAuraGameplayTags Tags = FAuraGameplayTags::Get();
	
	FGameplayEffectContextHandle EffectContextHandle = Params.SourceASC->MakeEffectContext();
	EffectContextHandle.AddSourceObject(Params.SourceASC->GetAvatarActor());
	SetDeathImpulse(EffectContextHandle,Params.DeathImpulse);
	SetKnockbackForce(EffectContextHandle, Params.KnockbackForce);
	FGameplayEffectSpecHandle EffectSpecHandle = Params.SourceASC->MakeOutgoingSpec(Params.DamageGameplayEffectClass, Params.Ability_Level, EffectContextHandle);
	
	//Assign the set by caller to spec.
	UAuraAbilitySystemLibrary::AssignTagSetByCallerMagnitude(EffectSpecHandle, Params.DamageType, Params.Base_Damage);
	UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(EffectSpecHandle, Tags.Debuff_Chance, Params.Debuff_Chance);
	UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(EffectSpecHandle, Tags.Debuff_Damage, Params.Debuff_Damage);
	UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(EffectSpecHandle, Tags.Debuff_Duration, Params.Debuff_Duration);
	UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(EffectSpecHandle, Tags.Debuff_Frequency, Params.Debuff_Frequency);


	Params.TargetASC->ApplyGameplayEffectSpecToSelf(*EffectSpecHandle.Data.Get());
	

	return EffectContextHandle;
}

TArray<FVector> UAuraAbilitySystemLibrary::SpawnRotateVectors(const FVector& Forward, const FVector& Axis, int32 NumOfProjectiles, float ProjectileSpread)
{
	TArray<FVector> SpawnVectors = TArray<FVector>();
	const FVector LeftForward = Forward.RotateAngleAxis(-ProjectileSpread / 2, FVector::UpVector);

	if (NumOfProjectiles > 1) {
		const float DeltaSpread = ProjectileSpread / (NumOfProjectiles - 1);

		for (int i = 0; i < NumOfProjectiles; ++i) {
			const FVector Direction = LeftForward.RotateAngleAxis(DeltaSpread * i, FVector::UpVector);
			SpawnVectors.Add(Direction);
		}
	}
	else {
		SpawnVectors.Add(Forward);
	}

	return SpawnVectors;
}





TArray<FRotator> UAuraAbilitySystemLibrary::SpawnRotateRotators(const FVector& Forward, const FVector& Axis, int32 NumOfProjectiles, float ProjectileSpread)
{
	TArray<FRotator> SpawnRotators = TArray<FRotator>();
	const FVector LeftForward = Forward.RotateAngleAxis(-ProjectileSpread / 2, FVector::UpVector);
	
	if (NumOfProjectiles > 1) {
		const float DeltaSpread = ProjectileSpread / (NumOfProjectiles - 1);

		for (int i = 0; i < NumOfProjectiles; ++i) {
			const FVector Direction = LeftForward.RotateAngleAxis(DeltaSpread * i, FVector::UpVector);
			SpawnRotators.Add(Direction.Rotation());
		}
	}
	else {
		SpawnRotators.Add(Forward.Rotation());
	}

	return SpawnRotators;
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

bool UAuraAbilitySystemLibrary::IsSuccessfulDebuff(const FGameplayEffectContextHandle& EffectContextHandle)
{
	if (const FAuraGameplayEffectContext* AuraEffectContextHandle
		= static_cast<const FAuraGameplayEffectContext*>(EffectContextHandle.Get())) {

		return AuraEffectContextHandle->IsSuccessfulDebuff();
	}
	return false;
}

float UAuraAbilitySystemLibrary::GetDebuffDamage(const FGameplayEffectContextHandle& EffectContextHandle)
{
	if (const FAuraGameplayEffectContext* AuraEffectContextHandle
		= static_cast<const FAuraGameplayEffectContext*>(EffectContextHandle.Get())) {

		return AuraEffectContextHandle->GetDebuffDamage();
	}
	return 0.0f;
}

float UAuraAbilitySystemLibrary::GetDebuffDuration(const FGameplayEffectContextHandle& EffectContextHandle)
{
	if (const FAuraGameplayEffectContext* AuraEffectContextHandle
		= static_cast<const FAuraGameplayEffectContext*>(EffectContextHandle.Get())) {

		return AuraEffectContextHandle->GetDebuffDuration();
	}
	return 0.0f;
}

float UAuraAbilitySystemLibrary::GetDebuffFrequency(const FGameplayEffectContextHandle& EffectContextHandle)
{
	if (const FAuraGameplayEffectContext* AuraEffectContextHandle
		= static_cast<const FAuraGameplayEffectContext*>(EffectContextHandle.Get())) {

		return AuraEffectContextHandle->GetDebuffFrequency();
	}
	return 0.0f;
}

FGameplayTag UAuraAbilitySystemLibrary::GetDamageType(const FGameplayEffectContextHandle& EffectContextHandle)
{
	if (const FAuraGameplayEffectContext* AuraEffectContextHandle
		= static_cast<const FAuraGameplayEffectContext*>(EffectContextHandle.Get())) {

		if (AuraEffectContextHandle->GetDamageType().IsValid()) {
			return *AuraEffectContextHandle->GetDamageType();
		}
	}
	return FGameplayTag();
}

FVector UAuraAbilitySystemLibrary::GetDeathImpulse(const FGameplayEffectContextHandle& EffectContextHandle)
{
	if (const FAuraGameplayEffectContext* AuraEffectContextHandle =
		static_cast<const FAuraGameplayEffectContext*>(EffectContextHandle.Get())) {

		return AuraEffectContextHandle->GetDeathImpulse();
	}
	return FVector::ZeroVector;
}

FVector UAuraAbilitySystemLibrary::GetKnockbackForce(const FGameplayEffectContextHandle& EffectContextHandle)
{
	if (const FAuraGameplayEffectContext* AuraEffectContextHandle =
		static_cast<const FAuraGameplayEffectContext*>(EffectContextHandle.Get())) {

		return AuraEffectContextHandle->GetKnockbackForce();
	}
	return FVector::ZeroVector;
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

void UAuraAbilitySystemLibrary::SetIsSuccessfulDebuff(UPARAM(ref)FGameplayEffectContextHandle& EffectContextHandle, bool bInSuccessfulDebuff)
{
	if (FAuraGameplayEffectContext* AuraEffectContextHandle =
		static_cast<FAuraGameplayEffectContext*>(EffectContextHandle.Get())) {

		AuraEffectContextHandle->SetIsSuccessfulDebuff(bInSuccessfulDebuff);
	}
}

void UAuraAbilitySystemLibrary::SetDebuffDamage(UPARAM(ref)FGameplayEffectContextHandle& EffectContextHandle, float InDamage)
{
	if (FAuraGameplayEffectContext* AuraEffectContextHandle =
		static_cast<FAuraGameplayEffectContext*>(EffectContextHandle.Get())) {

		AuraEffectContextHandle->SetDebuffDamage(InDamage);
	}
}

void UAuraAbilitySystemLibrary::SetDebuffDuration(UPARAM(ref)FGameplayEffectContextHandle& EffectContextHandle, float InDuration)
{
	if (FAuraGameplayEffectContext* AuraEffectContextHandle =
		static_cast<FAuraGameplayEffectContext*>(EffectContextHandle.Get())) {

		AuraEffectContextHandle->SetDebuffDuration(InDuration);
	}
}

void UAuraAbilitySystemLibrary::SetDebuffFrequency(UPARAM(ref)FGameplayEffectContextHandle& EffectContextHandle, float InFrequency)
{
	if (FAuraGameplayEffectContext* AuraEffectContextHandle =
		static_cast<FAuraGameplayEffectContext*>(EffectContextHandle.Get())) {

		AuraEffectContextHandle->SetDebuffFrequency(InFrequency);
	}
}

void UAuraAbilitySystemLibrary::SetDamageType(UPARAM(ref)FGameplayEffectContextHandle& EffectContextHandle, const FGameplayTag& DebuffDamageType)
{
	if (FAuraGameplayEffectContext* AuraEffectContextHandle =
		static_cast<FAuraGameplayEffectContext*>(EffectContextHandle.Get())) {

		TSharedPtr<FGameplayTag> InDamageType = MakeShared<FGameplayTag>(DebuffDamageType);
		AuraEffectContextHandle->SetDamageType(InDamageType);
	}
}

void UAuraAbilitySystemLibrary::SetDeathImpulse(UPARAM(ref)FGameplayEffectContextHandle& EffectContextHandle, const FVector& InDeathImpulse)
{
	if (FAuraGameplayEffectContext* AuraEffectContextHandle =
		static_cast<FAuraGameplayEffectContext*>(EffectContextHandle.Get())) {

		AuraEffectContextHandle->SetDeathImpulse(InDeathImpulse);
	}
}

void UAuraAbilitySystemLibrary::SetKnockbackForce(UPARAM(ref)FGameplayEffectContextHandle& EffectContextHandle, const FVector& InKnockbackForce)
{
	if (FAuraGameplayEffectContext* AuraEffectContextHandle =
		static_cast<FAuraGameplayEffectContext*>(EffectContextHandle.Get())) {

		AuraEffectContextHandle->SetKnockbackForce(InKnockbackForce);
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

void UAuraAbilitySystemLibrary::FindNumOfNearActos(int32 MaxNumOfTarget, const TArray<AActor*>& Actors, TArray<AActor*>& OutTargetActors, const FVector& Origin)
{
	// Priority Queue to find the MaxNumOfTarget-th of nearest actor;
	std::priority_queue<std::pair<double, AActor*>> PriorityQueueOfVectors;

	for (AActor* actor : Actors) {
		FVector ActorLocation = actor->GetActorLocation();
		double diff = (ActorLocation - Origin).Length();
		std::pair<double, AActor*> pair = std::make_pair(diff, actor);
		PriorityQueueOfVectors.push(pair);
	}

	for (int i = 1; i <= MaxNumOfTarget; ++i) {
		if (PriorityQueueOfVectors.empty()) return;
		std::pair<double, AActor*> tmp = PriorityQueueOfVectors.top();
		PriorityQueueOfVectors.pop();
		OutTargetActors.Add(tmp.second);
	}
}


bool UAuraAbilitySystemLibrary::IsFriend(AActor* FirstActor, AActor* SecondActor)
{
	const bool PlayerFriend = FirstActor->ActorHasTag("Player") && SecondActor->ActorHasTag("Player");
	const bool EnemyFriend = FirstActor->ActorHasTag("Enemy") && SecondActor->ActorHasTag("Enemy");

	return PlayerFriend || EnemyFriend;
}

