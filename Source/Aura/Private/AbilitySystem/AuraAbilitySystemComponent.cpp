// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/AuraAbilitySystemComponent.h"

#include "AbilitySystem/Abilities/AuraGameplayAbility.h"
#include "Interaction/PlayerInterface.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AuraGameplayTags.h"
#include "AbilitySystem/AuraAbilitySystemLibrary.h"
#include "AbilitySystem/Data/AbilityInfo.h"
#include "Interaction/PlayerInterface.h"
#include "AbilitySystem/Data/AbilityInfo.h"
#include "Game/LoadScreenSaveGame.h"



void UAuraAbilitySystemComponent::AbilitySetInfo()
{
	OnGameplayEffectAppliedDelegateToSelf.AddUObject(this, &UAuraAbilitySystemComponent::ClientEffectApplied);

}

void UAuraAbilitySystemComponent::AddCharacterAbilities(const TArray<TSubclassOf<UGameplayAbility>>& StartUpAbilities)
{
	for (auto GameplayAbility : StartUpAbilities) {
		FGameplayAbilitySpec GASpec = FGameplayAbilitySpec(GameplayAbility, 1);
		if (const UAuraGameplayAbility* AuraGA = Cast<UAuraGameplayAbility>(GASpec.Ability)) {
			GASpec.DynamicAbilityTags.AddTag(AuraGA->StartupInputTag);
			GASpec.DynamicAbilityTags.AddTag(FAuraGameplayTags::Get().Abilities_Status_Equipped);
			GiveAbility(GASpec);
		}
	}
	bIsStartupAbilityInitialized = true;
	AbilityInfoDelegate.Broadcast();
}

void UAuraAbilitySystemComponent::AddCharacterAbilitiesFromSaveData(ULoadScreenSaveGame* SaveGame)
{
	FAuraGameplayTags AuraTags = FAuraGameplayTags::Get();

	for (FSaveGameAbility Data : SaveGame->SaveGameAbilities) {
		const TSubclassOf<UGameplayAbility> LoadedAbilityClass = Data.GameplayAbility;
		FGameplayAbilitySpec LoadedAbilitySpec = FGameplayAbilitySpec(LoadedAbilityClass, Data.level);

		LoadedAbilitySpec.DynamicAbilityTags.AddTag(Data.AbilityStatus);
		LoadedAbilitySpec.DynamicAbilityTags.AddTag(Data.AbilitySlot);
		if (Data.AbilityType.MatchesTagExact(AuraTags.Abilities_Type_Offensive)) {
			GiveAbility(LoadedAbilitySpec);
		}
		else if (Data.AbilityType.MatchesTagExact(AuraTags.Abilities_Type_Passive)) {
			if (Data.AbilityStatus.MatchesTagExact(AuraTags.Abilities_Status_Equipped) ){
				GiveAbilityAndActivateOnce(LoadedAbilitySpec);
				MulticastActivatePassiveEffect(Data.AbilityTag, true);
			}
			else {
				GiveAbility(LoadedAbilitySpec);
			}
		}
	}
	bIsStartupAbilityInitialized = true;
	AbilityInfoDelegate.Broadcast();
}

void UAuraAbilitySystemComponent::AddCharacterPassiveAbilities(const TArray<TSubclassOf<UGameplayAbility>>& StartUpPassivepAbilities)
{
	for (auto GameplayAbility : StartUpPassivepAbilities) {
		FGameplayAbilitySpec GASpec = FGameplayAbilitySpec(GameplayAbility, 1);
		GASpec.DynamicAbilityTags.AddTag(FAuraGameplayTags::Get().Abilities_Status_Equipped);
		GiveAbilityAndActivateOnce(GASpec);
	}
}

void UAuraAbilitySystemComponent::AbilityTagPress(const FGameplayTag& Tag)
{
	if (!Tag.IsValid())return;
	FScopedAbilityListLock ActiveScopeLoc(*this);
	for (auto& spec : GetActivatableAbilities()) {
		if (spec.DynamicAbilityTags.HasTagExact(Tag)) {
			AbilitySpecInputPressed(spec);
			if (spec.IsActive()) {
				InvokeReplicatedEvent(
					EAbilityGenericReplicatedEvent::InputPressed,
					spec.Handle,
					spec.ActivationInfo.GetActivationPredictionKey()
				);
			}
		}
	}
}

void UAuraAbilitySystemComponent::AbilityTagHeld(const FGameplayTag& Tag)
{
	if (!Tag.IsValid())return;
	FScopedAbilityListLock ActiveScopeLoc(*this);
	for (auto& spec : GetActivatableAbilities()) {
		if (spec.DynamicAbilityTags.HasTagExact(Tag)) {
			AbilitySpecInputPressed(spec);
			if (!spec.IsActive()) {
				TryActivateAbility(spec.Handle);
			}
		}
	}
}

void UAuraAbilitySystemComponent::AbilityTagRelease(const FGameplayTag& Tag)
{
	if (!Tag.IsValid())return;
	FScopedAbilityListLock ActiveScopeLoc(*this);

	for (auto& spec : GetActivatableAbilities()) {
		if (spec.DynamicAbilityTags.HasTagExact(Tag) && spec.IsActive()) {
			AbilitySpecInputReleased(spec);
			InvokeReplicatedEvent(
				EAbilityGenericReplicatedEvent::InputReleased,
				spec.Handle,
				spec.ActivationInfo.GetActivationPredictionKey()
			);
		}
	}
}

void UAuraAbilitySystemComponent::ForEachAbility(const FForEachAbility& Delegate)
{
	FScopedAbilityListLock ActiveScopeLock(*this);
	for (const FGameplayAbilitySpec& AbilitySpec : GetActivatableAbilities()) {
		if (!Delegate.ExecuteIfBound(AbilitySpec)) {
			UE_LOG(LogTemp, Warning, TEXT("Failed to execute the delegate in %s."), *GetNameSafe(this));
		}
	}
}

void UAuraAbilitySystemComponent::UpgradeAttributeByPoint(const FGameplayTag& AttributeTag)
{
	if (GetAvatarActor()->Implements<UPlayerInterface>()) {
		if (IPlayerInterface::Execute_GetAttributePoints(GetAvatarActor()) > 0) {
			UpgradeServerAttribute(AttributeTag);
		}
	}
}

void UAuraAbilitySystemComponent::UpgradeServerAttribute_Implementation(const FGameplayTag& AttributeTag)
{
	FGameplayEventData Payload;
	Payload.EventTag = AttributeTag;
	Payload.EventMagnitude = 1.f;

	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(GetAvatarActor(), AttributeTag, Payload);
	if (GetAvatarActor()->Implements<UPlayerInterface>()) {
		IPlayerInterface::Execute_AddToAttributePoints(GetAvatarActor(), -1);
	}
}

void UAuraAbilitySystemComponent::ServerSpendSpellPoints_Implementation(const FGameplayTag& AbilityTag)
{
	if (FGameplayAbilitySpec* spec = GetSpecFromTag(AbilityTag)) {
		if (GetAvatarActor()->Implements<UPlayerInterface>()) {
			IPlayerInterface::Execute_AddToSpellPoints(GetAvatarActor(), -1);
		}

		FAuraGameplayTags AuraTags = FAuraGameplayTags::Get();
		FGameplayTag status = GetStatusTagBySpec(*spec);

		if (status.MatchesTagExact(AuraTags.Abilities_Status_Eligible)) {
			spec->DynamicAbilityTags.RemoveTag(AuraTags.Abilities_Status_Eligible);
			spec->DynamicAbilityTags.AddTag(AuraTags.Abilities_Status_Unlocked);
			status = AuraTags.Abilities_Status_Unlocked;
		}
		else if (status.MatchesTagExact(AuraTags.Abilities_Status_Equipped) ||
			     status.MatchesTagExact(AuraTags.Abilities_Status_Unlocked)) {
			spec->Level += 1;
		}
		ClientStatusChangeApplied(AbilityTag, status, spec->Level);
		MarkAbilitySpecDirty(*spec);
	}
}

void UAuraAbilitySystemComponent::ServerEquipAbility_Implementation(const FGameplayTag& AbilityTag, const FGameplayTag& SlotTag)
{
	if (FGameplayAbilitySpec* AbilitySpec = GetSpecFromTag(AbilityTag)) {
		const FAuraGameplayTags& Tags = FAuraGameplayTags::Get();
		const FGameplayTag& PrevSlot = GetSlotByAbilityTag(AbilityTag);
		const FGameplayTag& Status = GetStatusByAbilityTag(AbilityTag);

		const bool bStatusValid = Status == Tags.Abilities_Status_Equipped || Status == Tags.Abilities_Status_Unlocked;
		if (bStatusValid) {
			if (!SlotIsEmpty(SlotTag)) {
				FGameplayAbilitySpec* SpecWithSlot = GetSpecBySlot(SlotTag);
				if (SpecWithSlot) {
					//If Ability is not change,still equip to cancel the animation.

					if (AbilityTag.MatchesTagExact(GetAbilityTagBySpec(*SpecWithSlot))) {
						ClientEquipAbility(AbilityTag, Tags.Abilities_Status_Equipped, PrevSlot, SlotTag);
						return;
					}

					if (IsPassiveAbility(*SpecWithSlot)) {
						MulticastActivatePassiveEffect(AbilityTag, false);
						PassiveDeactivateDelegate.Broadcast(GetAbilityTagBySpec(*SpecWithSlot));
					}

					ClearSlot(SpecWithSlot);
				}
			}
			//Ability is not equip at all.
			if (!AbilityHasAnySlot(*AbilitySpec)) {
				if (IsPassiveAbility(*AbilitySpec)) {
					TryActivateAbility(AbilitySpec->Handle);
					MulticastActivatePassiveEffect(AbilityTag, true);
					AbilitySpec->DynamicAbilityTags.RemoveTag(GetStatusTagBySpec(*AbilitySpec));
					AbilitySpec->DynamicAbilityTags.AddTag(Tags.Abilities_Status_Equipped);
				}
			}
			AssignSlotToAbility(*AbilitySpec, SlotTag);
			MarkAbilitySpecDirty(*AbilitySpec);
			ClientEquipAbility(AbilityTag, Tags.Abilities_Status_Equipped, PrevSlot, SlotTag);
		}
	}
}

void UAuraAbilitySystemComponent::ClientEquipAbility_Implementation(const FGameplayTag& AbilityTag, const FGameplayTag& StatusTag, const FGameplayTag& PrevSlotTag, const FGameplayTag& SlotTag)
{
	AbilityEquipped.Broadcast(AbilityTag, StatusTag, PrevSlotTag, SlotTag);
}

FGameplayTag UAuraAbilitySystemComponent::GetAbilityTagBySpec(const FGameplayAbilitySpec& AbilitySpec)
{
	if (AbilitySpec.Ability) {
		for (FGameplayTag tag : AbilitySpec.Ability.Get()->AbilityTags) {
			if (tag.MatchesTag(FGameplayTag::RequestGameplayTag(FName("Abilities")))) {
				return tag;
			}
		}
	}
	return FGameplayTag();
}

FGameplayTag UAuraAbilitySystemComponent::GetInputTagBySpec(const FGameplayAbilitySpec& AbilitySpec)
{
	for (FGameplayTag tag : AbilitySpec.DynamicAbilityTags) {
		if(tag.MatchesTag(FGameplayTag::RequestGameplayTag(FName("InputTag")))) {
			return tag;
		}
	}
	return FGameplayTag();
}

FGameplayTag UAuraAbilitySystemComponent::GetStatusTagBySpec(const FGameplayAbilitySpec& AbilitySpec)
{
	for (FGameplayTag tag : AbilitySpec.DynamicAbilityTags) {
		if (tag.MatchesTag(FGameplayTag::RequestGameplayTag(FName("Abilities.Status")))) {
			return tag;
		}
	}
	
	return FGameplayTag();
}

FGameplayTag UAuraAbilitySystemComponent::GetSlotByAbilityTag(const FGameplayTag& AbilityTag)
{
	if (FGameplayAbilitySpec* spec = GetSpecFromTag(AbilityTag)) {
		return GetInputTagBySpec(*spec);
	}
	return FGameplayTag();
}

FGameplayTag UAuraAbilitySystemComponent::GetStatusByAbilityTag(const FGameplayTag& AbilityTag)
{
	if (FGameplayAbilitySpec* spec = GetSpecFromTag(AbilityTag)) {
		return GetStatusTagBySpec(*spec);
	}
	return FGameplayTag();
}

void UAuraAbilitySystemComponent::UpdateStatus(int32 level)
{
	UAbilityInfo* AbilityInfo = UAuraAbilitySystemLibrary::GetAbilityInfo(GetAvatarActor());
	for (const FAuraAbilityInfo& info : AbilityInfo->AbilityInformation) {
		if (!info.AbilityTag.IsValid()) continue;
		if (level < info.LevelRequirement) continue;

		if (GetSpecFromTag(info.AbilityTag) == nullptr) {
			FGameplayAbilitySpec spec = FGameplayAbilitySpec(info.Ability,1);
			spec.DynamicAbilityTags.AddTag(FAuraGameplayTags::Get().Abilities_Status_Eligible);
			GiveAbility(spec);
			MarkAbilitySpecDirty(spec);
			ClientStatusChangeApplied(info.AbilityTag, FAuraGameplayTags::Get().Abilities_Status_Eligible,1);
		}
	}
}

FGameplayAbilitySpec* UAuraAbilitySystemComponent::GetSpecFromTag(const FGameplayTag& AbilityTag)
{
	FScopedAbilityListLock lock(*this);
	for (FGameplayAbilitySpec& spec : GetActivatableAbilities()) {
		for (FGameplayTag tag : spec.Ability.Get()->AbilityTags) {
			if (tag.MatchesTagExact(AbilityTag)) {
				return &spec;
			}
		}
	}
	
	return nullptr;
}

bool UAuraAbilitySystemComponent::GetStringByAbilityTag(const FGameplayTag& AbilityTag, FString& OutCurrDescription, FString& OutNextDescription)
{
	if (!AbilityTag.IsValid()) return false;
	if (FGameplayAbilitySpec* AbilitySpec = GetSpecFromTag(AbilityTag)) {
		if (UAuraGameplayAbility* AuraAbility = Cast<UAuraGameplayAbility>(AbilitySpec->Ability)) {
			OutCurrDescription = AuraAbility->GetCurrentDescription(AbilitySpec->Level);
			OutNextDescription = AuraAbility->GetNextDescription(AbilitySpec->Level+1);
			return true;
		}
	}

	const UAbilityInfo* AbilityInfo = UAuraAbilitySystemLibrary::GetAbilityInfo(GetAvatarActor());
	OutCurrDescription = UAuraGameplayAbility::GetLockedSpellDescription(
		AbilityInfo->GetAbilityInfoByTag(AbilityTag).LevelRequirement
	);
	OutNextDescription = FString();
	return false;

}

void UAuraAbilitySystemComponent::ClearSlot(FGameplayAbilitySpec* Spec)
{
	const FGameplayTag& Slot = GetInputTagBySpec(*Spec);
	Spec->DynamicAbilityTags.RemoveTag(Slot);
}

void UAuraAbilitySystemComponent::ClearAbilitiesOfSlot(const FGameplayTag& Slot)
{
	FScopedAbilityListLock ActiveScopeLock(*this);

	for (FGameplayAbilitySpec spec : GetActivatableAbilities()) {
		if (AbilityHasSlot(spec, Slot)) {
			ClearSlot(&spec);
		}
	}
}


bool UAuraAbilitySystemComponent::SlotIsEmpty(const FGameplayTag& Slot)
{
	FScopedAbilityListLock Lock(*this);
	for (FGameplayAbilitySpec& spec : GetActivatableAbilities()) {
		if (AbilityHasSlot(spec, Slot)) {
			return false;
		}
	}
	return true;
}

bool UAuraAbilitySystemComponent::AbilityHasSlot(const FGameplayAbilitySpec& Spec, const FGameplayTag& Slot)
{
	return Spec.DynamicAbilityTags.HasTagExact(Slot);
}

bool UAuraAbilitySystemComponent::AbilityHasAnySlot(const FGameplayAbilitySpec& Spec)
{
	return Spec.DynamicAbilityTags.HasTag(FGameplayTag::RequestGameplayTag(FName("Input")));
}

FGameplayAbilitySpec* UAuraAbilitySystemComponent::GetSpecBySlot(const FGameplayTag& Slot)
{
	FScopedAbilityListLock Lock(*this);

	for (FGameplayAbilitySpec& spec: GetActivatableAbilities()) {
		if (AbilityHasSlot(spec, Slot)) {
			return &spec;
		}
	}
	return nullptr;
}

bool UAuraAbilitySystemComponent::IsPassiveAbility(const FGameplayAbilitySpec& AbilitySpec) const
{
	FAuraGameplayTags Tags = FAuraGameplayTags::Get();
	FGameplayTag AbilityTag = GetAbilityTagBySpec(AbilitySpec);
	UAbilityInfo* Info = UAuraAbilitySystemLibrary::GetAbilityInfo(GetAvatarActor());
	FAuraAbilityInfo AbilityInfo = Info->GetAbilityInfoByTag(AbilityTag);


	return AbilityInfo.AbilityType.MatchesTagExact(Tags.Abilities_Type_Passive);
}

void UAuraAbilitySystemComponent::AssignSlotToAbility(FGameplayAbilitySpec& AbilitySpec, const FGameplayTag& Slot)
{
	ClearSlot(&AbilitySpec);
	AbilitySpec.DynamicAbilityTags.AddTag(Slot);
}

void UAuraAbilitySystemComponent::MulticastActivatePassiveEffect_Implementation(const FGameplayTag& AbilityTag, bool bActivate)
{
	ActivatePassiveEffect.Broadcast(AbilityTag, bActivate);
}

void UAuraAbilitySystemComponent::ClientStatusChangeApplied_Implementation(const FGameplayTag& AbilityTag, const FGameplayTag& StatusTag,int32 level)
{
	StatusChangeDelegate.Broadcast(AbilityTag, StatusTag,level);
}

void UAuraAbilitySystemComponent::OnRep_ActivateAbilities()
{
	Super::OnRep_ActivateAbilities();

	if (!bIsStartupAbilityInitialized) {
		bIsStartupAbilityInitialized = true;
		AbilityInfoDelegate.Broadcast();
	}
}

void UAuraAbilitySystemComponent::ClientEffectApplied_Implementation(UAbilitySystemComponent* ASC, const FGameplayEffectSpec& GameplayEffectSpec, FActiveGameplayEffectHandle GameplayEffectHandle)
{
	FGameplayTagContainer TagContainer;
	GameplayEffectSpec.GetAllAssetTags(TagContainer);

	EffectTags.Broadcast(TagContainer);
}
