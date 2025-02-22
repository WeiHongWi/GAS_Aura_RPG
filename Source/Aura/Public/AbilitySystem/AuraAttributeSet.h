// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "AbilitySystemComponent.h"
#include "AuraAttributeSet.generated.h"

#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)
/**
 *
 */
USTRUCT()
struct FEffectProperties {
	GENERATED_BODY();

	FEffectProperties(){}

	FGameplayEffectContextHandle EffectContextHandle;
	
	UPROPERTY()
	UAbilitySystemComponent* ASCSource = nullptr;
	UPROPERTY()
	AActor* SourceAvatarActor = nullptr;
	UPROPERTY()
	AController* SourceController = nullptr;
	UPROPERTY()
	ACharacter* SourceCharacter = nullptr;
	
	UPROPERTY()
	UAbilitySystemComponent* ASCTarget = nullptr;
	UPROPERTY()
	AActor* TargetAvatarActor = nullptr;
	UPROPERTY()
	AController* TargetController = nullptr;
	UPROPERTY()
	ACharacter* TargetCharacter = nullptr;
	
};

template<class T>
using TStaticFuncPtr = typename TBaseStaticDelegateInstance<T, FDefaultDelegateUserPolicy>::FFuncPtr;

UCLASS()
class AURA_API UAuraAttributeSet : public UAttributeSet
{
	GENERATED_BODY()
public:
	UAuraAttributeSet();
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void PreAttributeChange(const FGameplayAttribute& Attribute,float& NewValue) override;
	virtual void PostGameplayEffectExecute(const FGameplayEffectModCallbackData& data) override;
	virtual void PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue) override;

	TMap<FGameplayTag, TStaticFuncPtr<FGameplayAttribute()>> TagsToAttribute;

	//Primary Attributes
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Strength, Category = "Primary Attributes")
	FGameplayAttributeData Strength;
	ATTRIBUTE_ACCESSORS(UAuraAttributeSet, Strength);

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Intelligence, Category = "Primary Attributes")
	FGameplayAttributeData Intelligence;
	ATTRIBUTE_ACCESSORS(UAuraAttributeSet, Intelligence);

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Resilience, Category = "Primary Attributes")
	FGameplayAttributeData Resilience;
	ATTRIBUTE_ACCESSORS(UAuraAttributeSet, Resilience);

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Vigor, Category = "Primary Attributes")
	FGameplayAttributeData Vigor;
	ATTRIBUTE_ACCESSORS(UAuraAttributeSet, Vigor);

	//Secondary Attributes
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Armor, Category = "Secondary Attributes")
	FGameplayAttributeData Armor;
	ATTRIBUTE_ACCESSORS(UAuraAttributeSet, Armor);

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Armor_Penetration, Category = "Secondary Attributes")
	FGameplayAttributeData Armor_Penetration;
	ATTRIBUTE_ACCESSORS(UAuraAttributeSet, Armor_Penetration);

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Block_Chance, Category = "Secondary Attributes")
	FGameplayAttributeData Block_Chance;
	ATTRIBUTE_ACCESSORS(UAuraAttributeSet, Block_Chance);

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Critical_Hit_Chance, Category = "Secondary Attributes")
	FGameplayAttributeData Critical_Hit_Chance;
	ATTRIBUTE_ACCESSORS(UAuraAttributeSet, Critical_Hit_Chance);

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Critical_Hit_Damage, Category = "Secondary Attributes")
	FGameplayAttributeData Critical_Hit_Damage;
	ATTRIBUTE_ACCESSORS(UAuraAttributeSet, Critical_Hit_Damage);

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Critical_Hit_Resistance, Category = "Secondary Attributes")
	FGameplayAttributeData Critical_Hit_Resistance;
	ATTRIBUTE_ACCESSORS(UAuraAttributeSet, Critical_Hit_Resistance);

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Mana_Regeneration, Category = "Secondary Attributes")
	FGameplayAttributeData Mana_Regeneration;
	ATTRIBUTE_ACCESSORS(UAuraAttributeSet, Mana_Regeneration);

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Health_Regeneration, Category = "Secondary Attributes")
	FGameplayAttributeData Health_Regeneration;
	ATTRIBUTE_ACCESSORS(UAuraAttributeSet, Health_Regeneration);

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_MaxHealth, Category = "Secondary Attributes")
	FGameplayAttributeData MaxHealth;
	ATTRIBUTE_ACCESSORS(UAuraAttributeSet, MaxHealth);

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_MaxMana, Category = "Secondary Attributes")
	FGameplayAttributeData MaxMana;
	ATTRIBUTE_ACCESSORS(UAuraAttributeSet, MaxMana);

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Fire_Resistance, Category = "Secondary Attributes")
	FGameplayAttributeData Fire_Resistance;
	ATTRIBUTE_ACCESSORS(UAuraAttributeSet, Fire_Resistance);

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Lightning_Resistance, Category = "Secondary Attributes")
	FGameplayAttributeData Lightning_Resistance;
	ATTRIBUTE_ACCESSORS(UAuraAttributeSet, Lightning_Resistance);

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Arcane_Resistance, Category = "Secondary Attributes")
	FGameplayAttributeData Arcane_Resistance;
	ATTRIBUTE_ACCESSORS(UAuraAttributeSet, Arcane_Resistance);

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Physics_Resistance, Category = "Secondary Attributes")
	FGameplayAttributeData Physics_Resistance;
	ATTRIBUTE_ACCESSORS(UAuraAttributeSet, Physics_Resistance);

	//Vital Attributes
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Health, Category = "Vital Attributes")
	FGameplayAttributeData Health;
	ATTRIBUTE_ACCESSORS(UAuraAttributeSet, Health);

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Mana, Category = "Vital Attributes")
	FGameplayAttributeData Mana;
	ATTRIBUTE_ACCESSORS(UAuraAttributeSet, Mana);

	// Meta Attribute
	UPROPERTY(BlueprintReadOnly, Category = "Meta Attributes")
	FGameplayAttributeData Incoming_Damage;
	ATTRIBUTE_ACCESSORS(UAuraAttributeSet, Incoming_Damage);

	UPROPERTY(BlueprintReadOnly, Category = "Meta Attributes")
	FGameplayAttributeData Incoming_Exp;
	ATTRIBUTE_ACCESSORS(UAuraAttributeSet, Incoming_Exp);
	


	UFUNCTION()
	void OnRep_Health(const FGameplayAttributeData& OldHealth) const;

	UFUNCTION()
	void OnRep_Mana(const FGameplayAttributeData& OldMana) const;

	//Primary Attribute
	UFUNCTION()
	void OnRep_Strength(const FGameplayAttributeData& OldStrength) const;

	UFUNCTION()
	void OnRep_Intelligence(const FGameplayAttributeData& OldIntelligence) const;

	UFUNCTION()
	void OnRep_Resilience(const FGameplayAttributeData& OldResilience) const;

	UFUNCTION()
	void OnRep_Vigor(const FGameplayAttributeData& OldVigor) const;

	//Secondary Attributes
	UFUNCTION()
	void OnRep_Armor(const FGameplayAttributeData& OldArmor) const;

	UFUNCTION()
	void OnRep_Armor_Penetration(const FGameplayAttributeData& OldArmor_Penetration) const;

	UFUNCTION()
	void OnRep_Block_Chance(const FGameplayAttributeData& OldBlock_Chance) const;

	UFUNCTION()
	void OnRep_Critical_Hit_Chance(const FGameplayAttributeData& OldCritical_Hit_Chance) const;

	UFUNCTION()
	void OnRep_Critical_Hit_Damage(const FGameplayAttributeData& OldCritical_Hit_Damage) const;

	UFUNCTION()
	void OnRep_Critical_Hit_Resistance(const FGameplayAttributeData& OldCritical_Hit_Resistance) const;

	UFUNCTION()
	void OnRep_Health_Regeneration(const FGameplayAttributeData& OldHealth_Regeneration) const;

	UFUNCTION()
	void OnRep_Mana_Regeneration(const FGameplayAttributeData& OldMana_Regeneration) const;

	UFUNCTION()
	void OnRep_MaxHealth(const FGameplayAttributeData& OldMaxHealth) const;

	UFUNCTION()
	void OnRep_MaxMana(const FGameplayAttributeData& OldMaxMana) const;

	UFUNCTION()
	void OnRep_Fire_Resistance(const FGameplayAttributeData& OldFireResistance) const;

	UFUNCTION()
	void OnRep_Lightning_Resistance(const FGameplayAttributeData& OldLightningResistance) const;

	UFUNCTION()
	void OnRep_Arcane_Resistance(const FGameplayAttributeData& OldArcaneResistance) const;

	UFUNCTION()
	void OnRep_Physics_Resistance(const FGameplayAttributeData& OldPhysicsResistance) const;
private:
	void SetEffectProperties(const FGameplayEffectModCallbackData& data, FEffectProperties& Props) const;
	void ShowFloatingText(const FEffectProperties& Props, float Damage, bool bBlockedHit, bool bCriticalHit) const;
	void SendEXPEvent(const FEffectProperties& Props);
	bool bTopOffHealth = false;
	bool bTopOffMana = false;
};
