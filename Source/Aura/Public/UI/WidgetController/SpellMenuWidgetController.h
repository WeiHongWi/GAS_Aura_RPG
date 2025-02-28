// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/WidgetController/AuraWidgetController.h"
#include "AuraGameplayTags.h"
#include "SpellMenuWidgetController.generated.h"

/**
 * 
 */

DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FSelectSpellGlobeSignature, bool, bSpendPoints, bool ,bEquip,FString,CurDes,FString,NextDes);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FWaitEquipButtonClicked, const FGameplayTag&, AbilityType);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FStopEquipButtonClicked, const FGameplayTag&, AbilityType);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FSpellGlobeReassignSignature, const FGameplayTag&, AbilityTag);

struct FSelectedAbility
{
	FGameplayTag Ability = FGameplayTag();
	FGameplayTag Status = FGameplayTag();
};

UCLASS(BlueprintType, Blueprintable)
class AURA_API USpellMenuWidgetController : public UAuraWidgetController
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable)
	virtual void BroadcastInitValue() override;
	virtual void BindCallbacksToDependencies() override;

	UPROPERTY(BlueprintAssignable, Category = "GAS|Attributes")
	FOnPlayerStateChangeSignature PointsChangeDelegate;

	UPROPERTY(BlueprintAssignable)
	FSelectSpellGlobeSignature SelectSpellGlobeDelegate;

	UPROPERTY(BlueprintAssignable)
	FWaitEquipButtonClicked EquipButtonClickedDelegate;
	
	UPROPERTY(BlueprintAssignable)
	FStopEquipButtonClicked EquipButtonStopDelegate;
	
	UPROPERTY(BlueprintAssignable)
	FSpellGlobeReassignSignature SpellGlobeReassignSignature;

	UFUNCTION(BlueprintCallable)
	void SelectSpellGlobe(const FGameplayTag& AbilityTag);

	UFUNCTION(BlueprintCallable)
	void SpendSpellPoints();

	UFUNCTION(BlueprintCallable)
	void DeselectSpellGlobe();

	UFUNCTION(BlueprintCallable)
	void EquipButtonClicked();

	UFUNCTION(BlueprintCallable)
	void SpellGlobeClicked(const FGameplayTag& SlotTag, const FGameplayTag& AbilityType);

	void OnAbilityEquip(
		const FGameplayTag& AbilityTag,
		const FGameplayTag& StatusTag,
		const FGameplayTag& PrevSlotTag,
		const FGameplayTag& SlotTag
	);
private:
	void SetBoolByStatus(const FGameplayTag StatusTag, int32 SpellPoints, bool& bSpendPoints, bool& bEquip);
	FSelectedAbility SelectedAbility = { FAuraGameplayTags::Get().Abilities_None,  FAuraGameplayTags::Get().Abilities_Status_Locked };
	int32 CurrentSpellPoints = 0;
	bool bWaitingForEquipButton = false;

	FGameplayTag SelectedSlot;
};
