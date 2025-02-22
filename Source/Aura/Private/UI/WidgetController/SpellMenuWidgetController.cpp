// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/WidgetController/SpellMenuWidgetController.h"

#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "AbilitySystem/Data/AbilityInfo.h"
#include "Player/AuraPlayerState.h"
#include "AuraGameplayTags.h"

void USpellMenuWidgetController::BroadcastInitValue()
{
	BroadcastAbilityInfo(); 
	PointsChangeDelegate.Broadcast(GetAuraPS()->GetSpellPoints());
}

void USpellMenuWidgetController::BindCallbacksToDependencies()
{
	GetAuraASC()->StatusChangeDelegate.AddLambda([this](const FGameplayTag& AbilityTag,const FGameplayTag& StatusTag,int32 level) {
		if (SelectedAbility.Ability.MatchesTagExact(AbilityTag))
		{
			SelectedAbility.Status = StatusTag;
			bool bEnableSpendPoints = false;
			bool bEnableEquip = false;
			SetBoolByStatus(StatusTag, CurrentSpellPoints, bEnableSpendPoints, bEnableEquip);
			
			FString CurrentDescription;
			FString NextDescription;
			GetAuraASC()->GetStringByAbilityTag(AbilityTag, CurrentDescription, NextDescription);
			SelectSpellGlobeDelegate.Broadcast(bEnableSpendPoints, bEnableEquip, CurrentDescription, NextDescription);
		}
		
		if (AbilityInfo) {
			FAuraAbilityInfo info = AbilityInfo->GetAbilityInfoByTag(AbilityTag);
			info.StatusTag = StatusTag;
			AbilityInfoDelegate.Broadcast(info);
		}
		});
	GetAuraASC()->AbilityEquipped.AddUObject(this,&USpellMenuWidgetController::OnAbilityEquip);

	GetAuraPS()->SpellPointsChange.AddLambda([this](int32 SpellPoints) {
		PointsChangeDelegate.Broadcast(SpellPoints);
		CurrentSpellPoints = SpellPoints;
		bool bEnableSpendPoints = false;
		bool bEnableEquip = false;
		SetBoolByStatus(SelectedAbility.Status, CurrentSpellPoints, bEnableSpendPoints, bEnableEquip);
		
		FString CurrentDescription;
		FString NextDescription;
		GetAuraASC()->GetStringByAbilityTag(SelectedAbility.Ability, CurrentDescription, NextDescription);
		SelectSpellGlobeDelegate.Broadcast(bEnableSpendPoints, bEnableEquip, CurrentDescription, NextDescription);
		});


}

void USpellMenuWidgetController::SelectSpellGlobe(const FGameplayTag& AbilityTag)
{
	if (bWaitingForEquipButton) {
		EquipButtonStopDelegate.Broadcast(AbilityInfo->GetAbilityInfoByTag(AbilityTag).AbilityType);
		bWaitingForEquipButton = false;
	}
	
	FAuraGameplayTags AuraTags = FAuraGameplayTags::Get();
	int32 SpellPoints = GetAuraPS()->GetSpellPoints();
	FGameplayTag StatusTag;

	bool bTagValid = AbilityTag.IsValid();
	bool bTagNone = AbilityTag.MatchesTag(AuraTags.Abilities_None);
	const FGameplayAbilitySpec* Spec = GetAuraASC()->GetSpecFromTag(AbilityTag);
	bool bSpecValid = (Spec != nullptr);

	if (!bTagValid || bTagNone || !bSpecValid) {
		StatusTag = AuraTags.Abilities_Status_Locked;
	}
	else {
		FGameplayTag tmp = GetAuraASC()->GetStatusTagBySpec(*Spec);
		StatusTag = tmp;
	}
	SelectedAbility.Ability = AbilityTag;
	SelectedAbility.Status = StatusTag;
	
	bool bSpendPoints = false;
	bool bEquip = false;
	SetBoolByStatus(StatusTag, SpellPoints, bSpendPoints, bEquip);

	FString CurrentDescription;
	FString NextDescription;
	GetAuraASC()->GetStringByAbilityTag(AbilityTag,CurrentDescription, NextDescription);
	SelectSpellGlobeDelegate.Broadcast(bSpendPoints, bEquip, CurrentDescription, NextDescription);

}

void USpellMenuWidgetController::SpendSpellPoints()
{
	if (GetAuraASC()) {
		GetAuraASC()->ServerSpendSpellPoints(SelectedAbility.Ability);
	}
}

void USpellMenuWidgetController::DeselectSpellGlobe()
{
	if (bWaitingForEquipButton) {
		EquipButtonStopDelegate.Broadcast(AbilityInfo->GetAbilityInfoByTag(SelectedAbility.Ability).AbilityType);
		bWaitingForEquipButton = false;
	}
	
	
	FAuraGameplayTags AuraTags = FAuraGameplayTags::Get();
	SelectedAbility.Ability = AuraTags.Abilities_None;
	SelectedAbility.Status = AuraTags.Abilities_Status_Locked;

	SelectSpellGlobeDelegate.Broadcast(false, false, FString(), FString());
}

void USpellMenuWidgetController::EquipButtonClicked()
{
	const FGameplayTag AbilityType = AbilityInfo->GetAbilityInfoByTag(SelectedAbility.Ability).AbilityType;
	EquipButtonClickedDelegate.Broadcast(AbilityType);
	bWaitingForEquipButton = true;

	FGameplayTag Status = GetAuraASC()->GetStatusByAbilityTag(SelectedAbility.Ability);
	if (Status == FAuraGameplayTags::Get().Abilities_Status_Equipped) {
		SelectedSlot = GetAuraASC()->GetInputTagByAbilityTag(SelectedAbility.Ability);
	}
}

void USpellMenuWidgetController::SpellGlobeClicked(const FGameplayTag& SlotTag, const FGameplayTag& AbilityType)
{
	if (!bWaitingForEquipButton) return;
	const FGameplayTag& SelectedSpellType = AbilityInfo->GetAbilityInfoByTag(SelectedAbility.Ability).AbilityType;
	if (!SelectedSpellType.MatchesTagExact(AbilityType)) return;

	GetAuraASC()->ServerEquipAbility(SelectedAbility.Ability,SlotTag);
}


void USpellMenuWidgetController::OnAbilityEquip(const FGameplayTag& AbilityTag, const FGameplayTag& StatusTag, const FGameplayTag& PrevSlotTag, const FGameplayTag& SlotTag)
{
	bWaitingForEquipButton = false;
	FAuraGameplayTags Tags = FAuraGameplayTags::Get();
	
	// Remove the previous slot
	FAuraAbilityInfo OldAbilityInfo;
	OldAbilityInfo.AbilityTag = Tags.Abilities_None;
	OldAbilityInfo.StatusTag = Tags.Abilities_Status_Unlocked;
	OldAbilityInfo.InputTag = PrevSlotTag;
	AbilityInfoDelegate.Broadcast(OldAbilityInfo);

	//Broadcast the new slot.
	FAuraAbilityInfo CurrentAbilityInfo = AbilityInfo->GetAbilityInfoByTag(AbilityTag);
	CurrentAbilityInfo.StatusTag = StatusTag;
	CurrentAbilityInfo.InputTag = SlotTag;
	AbilityInfoDelegate.Broadcast(CurrentAbilityInfo);

	// Finish equip and end the animation of regional pick hint.
	EquipButtonStopDelegate.Broadcast(AbilityInfo->GetAbilityInfoByTag(AbilityTag).AbilityType);
}

void USpellMenuWidgetController::SetBoolByStatus(const FGameplayTag StatusTag, int32 SpellPoints, bool& bSpendPoints, bool& bEquip)
{
	FAuraGameplayTags AuraTags = FAuraGameplayTags::Get();

	bSpendPoints = false;
	bEquip = false;
	if (StatusTag.MatchesTagExact(AuraTags.Abilities_Status_Equipped)) {
		bEquip = true;
		if (SpellPoints > 0) {
			bSpendPoints = true;
		}
	}
	else if (StatusTag.MatchesTagExact(AuraTags.Abilities_Status_Eligible)) {
		if (SpellPoints > 0) {
			bSpendPoints = true;
		}
	}
	else if (StatusTag.MatchesTagExact(AuraTags.Abilities_Status_Unlocked)) {
		bEquip = true;
		if (SpellPoints > 0) {
			bSpendPoints = true;
		}
	}
}
