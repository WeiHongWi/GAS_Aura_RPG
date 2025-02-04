// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/WidgetController/OverlayAuraWidgetController.h"
#include "AbilitySystem/AuraAttributeSet.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "AbilitySystem/Data/AbilityInfo.h"
#include "Player/AuraPlayerState.h"
#include "AbilitySystem/Data/LevelupInfo.h"


void UOverlayAuraWidgetController::BroadcastInitValue()
{
	UAuraAttributeSet* AuraAS = CastChecked<UAuraAttributeSet>(AttributeSet);

	OnHealthChange.Broadcast(AuraAS->GetHealth());
	OnManaChange.Broadcast(AuraAS->GetMana());
	OnMaxHealthChange.Broadcast(AuraAS->GetMaxHealth());
	OnMaxManaChange.Broadcast(AuraAS->GetMaxMana());
}

void UOverlayAuraWidgetController::BindCallbacksToDependencies()
{
	AAuraPlayerState* AuraPS = CastChecked<AAuraPlayerState>(PlayerState);
	AuraPS->ExpChange.AddUObject(this,&UOverlayAuraWidgetController::OnExpChanged);
	
	const UAuraAttributeSet* AuraAS = CastChecked<UAuraAttributeSet>(AttributeSet);
	

	AbilitySystemComp->GetGameplayAttributeValueChangeDelegate(AuraAS->GetHealthAttribute())
		.AddLambda(
			[this](const FOnAttributeChangeData& data) {
				OnHealthChange.Broadcast(data.NewValue);
			}
		);

	AbilitySystemComp->GetGameplayAttributeValueChangeDelegate(AuraAS->GetManaAttribute())
		.AddLambda(
			[this](const FOnAttributeChangeData& data) {
				OnManaChange.Broadcast(data.NewValue);
			});

	AbilitySystemComp->GetGameplayAttributeValueChangeDelegate(AuraAS->GetMaxHealthAttribute())
		.AddLambda(
			[this](const FOnAttributeChangeData& data) {
				OnMaxHealthChange.Broadcast(data.NewValue);
			}
		);

	AbilitySystemComp->GetGameplayAttributeValueChangeDelegate(AuraAS->GetMaxManaAttribute())
		.AddLambda(
			[this](const FOnAttributeChangeData& data) {
				OnMaxManaChange.Broadcast(data.NewValue);
			});

	if (UAuraAbilitySystemComponent* AuraASC = 
		Cast<UAuraAbilitySystemComponent>(AbilitySystemComp)) {
		if (AuraASC->bIsStartupAbilityInitialized) {
			OnInitializeStartupAbilities(AuraASC);
		}
		else {
			AuraASC->AbilityInfoDelegate.AddUObject(
				this,
				&UOverlayAuraWidgetController::OnInitializeStartupAbilities
			);
		}
		AuraASC->EffectTags.AddLambda(
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

void UOverlayAuraWidgetController::OnInitializeStartupAbilities(UAuraAbilitySystemComponent* AuraAbilitySystemComponent)
{
	if (!AuraAbilitySystemComponent->bIsStartupAbilityInitialized) return;

	FForEachAbility ForEachDelegate;
	ForEachDelegate.BindLambda(
		[this, AuraAbilitySystemComponent](const FGameplayAbilitySpec& AbilitySpec) {
			FAuraAbilityInfo info = AbilityInfo->GetAbilityInfoByTag(AuraAbilitySystemComponent->GetAbilityTagBySpec(AbilitySpec));
			info.InputTag = AuraAbilitySystemComponent->GetInputTagBySpec(AbilitySpec);
			AbilityInfoDelegate.Broadcast(info);
		}
	);
	AuraAbilitySystemComponent->ForEachAbility(ForEachDelegate);
}

void UOverlayAuraWidgetController::OnExpChanged(int32 exp)
{
	const AAuraPlayerState* AuraPS = CastChecked<AAuraPlayerState>(PlayerState);
	ULevelupInfo* LevelupInfo = AuraPS->LevelupInfo;

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




