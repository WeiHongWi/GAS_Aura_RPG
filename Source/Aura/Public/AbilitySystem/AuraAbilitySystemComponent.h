// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "AuraAbilitySystemComponent.generated.h"


DECLARE_MULTICAST_DELEGATE_OneParam(FEffectTags,FGameplayTagContainer&);
DECLARE_MULTICAST_DELEGATE(FAbilityInfo);
DECLARE_DELEGATE_OneParam(FForEachAbility, const FGameplayAbilitySpec&);
DECLARE_MULTICAST_DELEGATE_ThreeParams(FStatusChangeDelegate, const FGameplayTag&, const FGameplayTag&,int32);
DECLARE_MULTICAST_DELEGATE_FourParams(FAbilityEquipped, const FGameplayTag& /*AbilityTag*/, const FGameplayTag& /*Status*/, const FGameplayTag& /*Slot*/, const FGameplayTag& /*PrevSlot*/);
DECLARE_MULTICAST_DELEGATE_OneParam(FPassiveDeactivate, const FGameplayTag& /*AbilityTag*/);
DECLARE_MULTICAST_DELEGATE_TwoParams(FPassiveSpellActivate, const FGameplayTag& /*AbilityTag*/,bool /*bActivate*/);
/**
 * 
 */


UCLASS()
class AURA_API UAuraAbilitySystemComponent : public UAbilitySystemComponent
{
	GENERATED_BODY()

public:
	void AbilitySetInfo();

	bool bIsStartupAbilityInitialized = false;

	FEffectTags EffectTags;
	FAbilityInfo AbilityInfoDelegate;
	FStatusChangeDelegate StatusChangeDelegate;
	FAbilityEquipped AbilityEquipped;
	FPassiveDeactivate PassiveDeactivateDelegate;
	FPassiveSpellActivate ActivatePassiveEffect;

	void AddCharacterAbilities(const TArray<TSubclassOf<UGameplayAbility>>& StartUpAbilities);
	void AddCharacterPassiveAbilities(const TArray<TSubclassOf<UGameplayAbility>>& StartUpPassivepAbilities);

	void AbilityTagPress(const FGameplayTag& Tag);
	void AbilityTagHeld(const FGameplayTag& Tag);
	void AbilityTagRelease(const FGameplayTag& Tag);
	void ForEachAbility(const FForEachAbility& Delegate);
	
	void UpgradeAttributeByPoint(const FGameplayTag& AttributeTag);
	
	UFUNCTION(Server,Reliable)
	void UpgradeServerAttribute(const FGameplayTag& AttributeTag);

	UFUNCTION(Server, Reliable)
	void ServerSpendSpellPoints(const FGameplayTag& AbilityTag);

	UFUNCTION(Server, Reliable)
	void ServerEquipAbility(const FGameplayTag& AbilityTag,const FGameplayTag& SlotTag);

	UFUNCTION(Client, Reliable)
	void ClientEquipAbility(
		const FGameplayTag& AbilityTag,
		const FGameplayTag& StatusTag,
		const FGameplayTag& PrevSlotTag,
		const FGameplayTag& SlotTag
	);

	UFUNCTION(NetMulticast, Unreliable)
	void MulticastActivatePassiveEffect(const FGameplayTag& AbilityTag, bool bActivate);

	static FGameplayTag GetAbilityTagBySpec(const FGameplayAbilitySpec& AbilitySpec);
	static FGameplayTag GetInputTagBySpec(const FGameplayAbilitySpec& AbilitySpec);
	static FGameplayTag GetStatusTagBySpec(const FGameplayAbilitySpec& AbilitySpec);
	FGameplayTag GetSlotByAbilityTag(const FGameplayTag& AbilityTag);
	FGameplayTag GetStatusByAbilityTag(const FGameplayTag& AbilityTag);

	void UpdateStatus(int32 level);
	FGameplayAbilitySpec* GetSpecFromTag(const FGameplayTag& AbilityTag);
	bool GetStringByAbilityTag(const FGameplayTag& AbilityTag, FString& OutCurrDescription, FString& OutNextDescription);
protected:
	static void ClearSlot(FGameplayAbilitySpec* Spec);
	void ClearAbilitiesOfSlot(const FGameplayTag& Slot);
	bool SlotIsEmpty(const FGameplayTag& Slot);
	static bool AbilityHasSlot(const FGameplayAbilitySpec& Spec, const FGameplayTag& Slot);
	static bool AbilityHasAnySlot(const FGameplayAbilitySpec& Spec);
	FGameplayAbilitySpec* GetSpecBySlot(const FGameplayTag& Slot);
	bool IsPassiveAbility(const FGameplayAbilitySpec& AbilitySpec) const;
	void AssignSlotToAbility(FGameplayAbilitySpec& AbilitySpec, const FGameplayTag& Slot);

	UFUNCTION(Client,Reliable)
	void ClientEffectApplied(
		UAbilitySystemComponent* ASC,
		const FGameplayEffectSpec& GameplayEffectSpec,
	    FActiveGameplayEffectHandle GameplayEffectHandle
	);

	UFUNCTION(Client, Reliable)
	void ClientStatusChangeApplied(const FGameplayTag& AbilityTag,const FGameplayTag& StatusTag,int32 level);

	virtual void OnRep_ActivateAbilities() override;
};
