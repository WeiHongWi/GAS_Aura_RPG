// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/AuraAttributeSet.h"

#include "GameplayEffectExtension.h"
#include "GameFramework/Character.h"
#include "AbilitySystemBlueprintLibrary.h"
#include <Net/UnrealNetwork.h>
#include "Aura/Aura.h"
#include "AuraGameplayTags.h"
#include "Interaction/CombatInterface.h"
#include "Player/AuraPlayerController.h"
#include "Kismet/GameplayStatics.h"


UAuraAttributeSet::UAuraAttributeSet()
{
	const FAuraGameplayTags& GameplayTag = FAuraGameplayTags::Get();

	// Primary Attribute
	TagsToAttribute.Add(GameplayTag.Attributes_Primary_Strength, GetStrengthAttribute);
	TagsToAttribute.Add(GameplayTag.Attributes_Primary_Intelligence, GetIntelligenceAttribute);
	TagsToAttribute.Add(GameplayTag.Attributes_Primary_Resilience, GetResilienceAttribute);
	TagsToAttribute.Add(GameplayTag.Attributes_Primary_Vigor, GetVigorAttribute);

	//Secondary Attribute
	TagsToAttribute.Add(GameplayTag.Attributes_Secondary_Armor, GetArmorAttribute);
	TagsToAttribute.Add(GameplayTag.Attributes_Secondary_ArmorPenetration, GetArmor_PenetrationAttribute);
	TagsToAttribute.Add(GameplayTag.Attributes_Secondary_BlockChance, GetBlock_ChanceAttribute);
	TagsToAttribute.Add(GameplayTag.Attributes_Secondary_CriticalHitChance, GetCritical_Hit_ChanceAttribute);
	TagsToAttribute.Add(GameplayTag.Attributes_Secondary_CriticalHitDamage, GetCritical_Hit_DamageAttribute);
	TagsToAttribute.Add(GameplayTag.Attributes_Secondary_CriticalHitResistance, GetCritical_Hit_ResistanceAttribute);
	TagsToAttribute.Add(GameplayTag.Attributes_Secondary_HealthRegeneration, GetHealth_RegenerationAttribute);
	TagsToAttribute.Add(GameplayTag.Attributes_Secondary_ManaRegeneration, GetMana_RegenerationAttribute);
	TagsToAttribute.Add(GameplayTag.Attributes_Secondary_MaxHealth, GetMaxHealthAttribute);
	TagsToAttribute.Add(GameplayTag.Attributes_Secondary_MaxMana, GetMaxManaAttribute);
}

void UAuraAttributeSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	//Primary Attributes
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, Strength, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, Intelligence, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, Resilience, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, Vigor, COND_None, REPNOTIFY_Always);
	
	//Secondary Attributes
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, Armor, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, Armor_Penetration, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, Block_Chance, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, Critical_Hit_Chance, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, Critical_Hit_Damage, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, Critical_Hit_Resistance, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, Health_Regeneration, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, Mana_Regeneration, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, MaxHealth, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, MaxMana, COND_None, REPNOTIFY_Always);
	//Vital Attributes
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet,Health,COND_None,REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, Mana, COND_None, REPNOTIFY_Always);
	
}

void UAuraAttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeBaseChange(Attribute, NewValue);

	if (Attribute == GetHealthAttribute()) {
		NewValue = FMath::Clamp(NewValue, 0.f, GetMaxHealth());
	}
	if (Attribute == GetManaAttribute()) {
		NewValue = FMath::Clamp(NewValue, 0.f, GetMaxMana());
	}
}
void UAuraAttributeSet::OnRep_Armor(const FGameplayAttributeData& OldArmor) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, Armor, OldArmor);
}
void UAuraAttributeSet::OnRep_Armor_Penetration(const FGameplayAttributeData& OldArmor_Penetration) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, Armor_Penetration, OldArmor_Penetration);
}
void UAuraAttributeSet::OnRep_Block_Chance(const FGameplayAttributeData& OldBlock_Chance) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, Block_Chance, OldBlock_Chance);
}
void UAuraAttributeSet::OnRep_Critical_Hit_Chance(const FGameplayAttributeData& OldCritical_Hit_Chance) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, Critical_Hit_Chance, OldCritical_Hit_Chance);
}
void UAuraAttributeSet::OnRep_Critical_Hit_Damage(const FGameplayAttributeData& OldCritical_Hit_Damage) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, Critical_Hit_Damage, OldCritical_Hit_Damage);
}
void UAuraAttributeSet::OnRep_Critical_Hit_Resistance(const FGameplayAttributeData& OldCritical_Hit_Resistance) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, Critical_Hit_Resistance, OldCritical_Hit_Resistance);
}
void UAuraAttributeSet::OnRep_Health_Regeneration(const FGameplayAttributeData& OldHealth_Regeneration) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, Health_Regeneration, OldHealth_Regeneration);
}
void UAuraAttributeSet::OnRep_Mana_Regeneration(const FGameplayAttributeData& OldMana_Regeneration) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, Mana_Regeneration, OldMana_Regeneration);
}
void UAuraAttributeSet::OnRep_MaxHealth(const FGameplayAttributeData& OldMaxHealth) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, MaxHealth, OldMaxHealth);
}
void UAuraAttributeSet::OnRep_MaxMana(const FGameplayAttributeData& OldMaxMana) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, MaxMana, OldMaxMana);
}
void UAuraAttributeSet::SetEffectProperties(const FGameplayEffectModCallbackData& data, FEffectProperties& Props) const
{
	// ~Begin source
	Props.EffectContextHandle = data.EffectSpec.GetContext();
	Props.ASCSource = Props.EffectContextHandle.GetOriginalInstigatorAbilitySystemComponent();

	if (IsValid(Props.ASCSource) && Props.ASCSource->AbilityActorInfo.IsValid()
		&& Props.ASCSource->AbilityActorInfo->AvatarActor.IsValid()) {
		Props.SourceAvatarActor = Props.ASCSource->AbilityActorInfo->AvatarActor.Get();
		Props.SourceController = Props.ASCSource->AbilityActorInfo->PlayerController.Get();

		if (Props.SourceController == nullptr && Props.SourceAvatarActor != nullptr) {
			if (const APawn* Pawn = Cast<APawn>(Props.SourceAvatarActor)) {
				Props.SourceController = Pawn->GetController();
			}
		}
		if (Props.SourceController) {
			Props.SourceCharacter = Cast<ACharacter>(Props.SourceController->GetPawn());
		}

	}
	//~End Source

	//~Begin Target
	if (data.Target.AbilityActorInfo.IsValid() &&
		data.Target.AbilityActorInfo->AvatarActor.IsValid()) {

		Props.TargetAvatarActor = data.Target.AbilityActorInfo->AvatarActor.Get();
		Props.TargetController = data.Target.AbilityActorInfo->PlayerController.Get();
		Props.TargetCharacter = Cast<ACharacter>(Props.TargetAvatarActor);
		Props.ASCTarget = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(Props.TargetAvatarActor);
	}

	//~End Target
}
void UAuraAttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& data)
{
	//Source: causer of effect, Target: target of effect
	Super::PostGameplayEffectExecute(data);
	
	struct FEffectProperties EffectProperties;
	SetEffectProperties(data, EffectProperties);
	
	if (data.EvaluatedData.Attribute == GetHealthAttribute()) {
		SetHealth(FMath::Clamp(GetHealth(), 0.f, GetMaxHealth()));
	}

	if (data.EvaluatedData.Attribute == GetManaAttribute()) {
		SetMana(FMath::Clamp(GetMana(), 0.f, GetMaxMana()));
	}

	if (data.EvaluatedData.Attribute == GetIncoming_DamageAttribute()) {
		const float LocalIncomingDamage = GetIncoming_Damage();
		SetIncoming_Damage(0.f);
		if (LocalIncomingDamage > 0.f) {
			const float NewHealth = GetHealth() - LocalIncomingDamage;
			SetHealth(FMath::Clamp(NewHealth, 0.f, GetMaxHealth()));
			
			const bool bFatal = NewHealth <= 0.f;
			if (!bFatal) {
				FGameplayTagContainer TagContainer;
				TagContainer.AddTag(FAuraGameplayTags::Get().HitReact);
				EffectProperties.ASCTarget->TryActivateAbilitiesByTag(TagContainer);
			}
			else {
				ICombatInterface* CombatInterface = Cast<ICombatInterface>(EffectProperties.TargetAvatarActor);
				if (CombatInterface) {
					CombatInterface->Die();
				}
			}
			//Call the damage text widget to it.
			
			if (EffectProperties.SourceCharacter != EffectProperties.TargetCharacter) {
				if (AAuraPlayerController* PC = Cast<AAuraPlayerController>
					(UGameplayStatics::GetPlayerController(EffectProperties.SourceCharacter,0))){
					PC->ShowDamageNumber(LocalIncomingDamage, EffectProperties.TargetCharacter);
				}
			}
		}
	}
}



void UAuraAttributeSet::OnRep_Health(const struct FGameplayAttributeData& OldHealth) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, Health, OldHealth);
}

void UAuraAttributeSet::OnRep_Mana(const FGameplayAttributeData& OldMana) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, Mana, OldMana);
}

void UAuraAttributeSet::OnRep_Strength(const FGameplayAttributeData& OldStrength) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, Strength, OldStrength);
}

void UAuraAttributeSet::OnRep_Intelligence(const FGameplayAttributeData& OldIntelligence) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, Intelligence, OldIntelligence);
}

void UAuraAttributeSet::OnRep_Resilience(const FGameplayAttributeData& OldResilience) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, Resilience, OldResilience);
}

void UAuraAttributeSet::OnRep_Vigor(const FGameplayAttributeData& OldVigor) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, Vigor, OldVigor);
}



