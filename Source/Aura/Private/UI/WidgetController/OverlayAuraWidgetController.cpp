// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/WidgetController/OverlayAuraWidgetController.h"
#include "AbilitySystem/AuraAttributeSet.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "AbilitySystem/Data/AbilityInfo.h"
#include "Player/AuraPlayerState.h"
#include "AbilitySystem/Data/LevelupInfo.h"
#include "AuraGameplayTags.h"


void UOverlayAuraWidgetController::BroadcastInitValue()
{
	OnHealthChange.Broadcast(GetAuraAS()->GetHealth());
	OnManaChange.Broadcast(GetAuraAS()->GetMana());
	OnMaxHealthChange.Broadcast(GetAuraAS()->GetMaxHealth());
	OnMaxManaChange.Broadcast(GetAuraAS()->GetMaxMana());
}

void UOverlayAuraWidgetController::BindCallbacksToDependencies()
{
	GetAuraPS()->ExpChange.AddUObject(this, &UOverlayAuraWidgetController::OnExpChanged);
	
	GetAuraPS()->LevelChange.AddLambda(
		[this](int32 NewLevel) {
			OnPlayerStateChangeDelegate.Broadcast(NewLevel);
		});


	AbilitySystemComp->GetGameplayAttributeValueChangeDelegate(GetAuraAS()->GetHealthAttribute())
		.AddLambda(
			[this](const FOnAttributeChangeData& data) {
				OnHealthChange.Broadcast(data.NewValue);
			}
		);

	AbilitySystemComp->GetGameplayAttributeValueChangeDelegate(GetAuraAS()->GetManaAttribute())
		.AddLambda(
			[this](const FOnAttributeChangeData& data) {
				OnManaChange.Broadcast(data.NewValue);
			});

	AbilitySystemComp->GetGameplayAttributeValueChangeDelegate(GetAuraAS()->GetMaxHealthAttribute())
		.AddLambda(
			[this](const FOnAttributeChangeData& data) {
				OnMaxHealthChange.Broadcast(data.NewValue);
			}
		);

	AbilitySystemComp->GetGameplayAttributeValueChangeDelegate(GetAuraAS()->GetMaxManaAttribute())
		.AddLambda(
			[this](const FOnAttributeChangeData& data) {
				OnMaxManaChange.Broadcast(data.NewValue);
			});

	if (GetAuraASC()) {
		GetAuraASC()->AbilityEquipped.AddUObject(this, &UOverlayAuraWidgetController::OnAbilityEquip);
		if (GetAuraASC()->bIsStartupAbilityInitialized) {
			BroadcastAbilityInfo();
		}
		else {
			GetAuraASC()->AbilityInfoDelegate.AddUObject(
				this,
				&UOverlayAuraWidgetController::BroadcastAbilityInfo
			);
		}
		GetAuraASC()->EffectTags.AddLambda(
			[this](const FGameplayTagContainer& EffectTag) {
				for (const FGameplayTag& Tag : EffectTag) {
					FGameplayTag Req_Tag = FGameplayTag::RequestGameplayTag(FName("Message"));
					if (Tag.MatchesTag(Req_Tag)) {
						const FUIWidgetRow* Row = GetDatatableRowByTag<FUIWidgetRow>(WidgetDT, Tag);
						MessageWidgetDelegate.Broadcast(*Row);
					}
				}
			}
		);
	}
}


void UOverlayAuraWidgetController::OnAbilityEquip(const FGameplayTag& AbilityTag, const FGameplayTag& StatusTag, const FGameplayTag& PrevSlotTag, const FGameplayTag& SlotTag)
{
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
}

void UOverlayAuraWidgetController::OnExpChanged(int32 exp)
{
	ULevelupInfo* LevelupInfo = GetAuraPS()->LevelupInfo;

	checkf(LevelupInfo, TEXT("Please put the info to bp_auraplayerstate"));

	const int32 level = LevelupInfo->FindLevelFromAmountOfExp(exp);
	const int32 Maxlevel = LevelupInfo->LevelupInfo.Num();

	if (level > 0 && level <= Maxlevel) {
		const int32 Cur_Requirement =
			LevelupInfo->LevelupInfo[level].RequiredExperience;
		const int32 Pre_Requirement =
			LevelupInfo->LevelupInfo[level - 1].RequiredExperience;

		const int32 Local = exp - Pre_Requirement;
		const int32 Global = Cur_Requirement - Pre_Requirement;

		float ExpPercent = static_cast<float>(Local) / static_cast<float>(Global);

		OnXpPercentChangeDelegate.Broadcast(ExpPercent);
	}
}




