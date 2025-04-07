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
#include "AbilitySystem/AuraAbilitySystemLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Interaction/PlayerInterface.h"
#include "AuraAbilityTypes.h"
#include "GameplayEffectComponents/TargetTagsGameplayEffectComponent.h"



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
	TagsToAttribute.Add(GameplayTag.Attributes_Resistance_Fire, GetFire_ResistanceAttribute);
	TagsToAttribute.Add(GameplayTag.Attributes_Resistance_Lightning, GetLightning_ResistanceAttribute);
	TagsToAttribute.Add(GameplayTag.Attributes_Resistance_Arcane, GetArcane_ResistanceAttribute);
	TagsToAttribute.Add(GameplayTag.Attributes_Resistance_Physics, GetPhysics_ResistanceAttribute);

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

	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, Fire_Resistance, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, Lightning_Resistance, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, Arcane_Resistance, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, Physics_Resistance, COND_None, REPNOTIFY_Always);
	
	
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
void UAuraAttributeSet::OnRep_Fire_Resistance(const FGameplayAttributeData& OldFireResistance) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, Fire_Resistance, OldFireResistance);
}
void UAuraAttributeSet::OnRep_Lightning_Resistance(const FGameplayAttributeData& OldLightningResistance) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, Lightning_Resistance, OldLightningResistance);
}
void UAuraAttributeSet::OnRep_Arcane_Resistance(const FGameplayAttributeData& OldArcaneResistance) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, Arcane_Resistance, OldArcaneResistance);
}
void UAuraAttributeSet::OnRep_Physics_Resistance(const FGameplayAttributeData& OldPhysicsResistance) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, Physics_Resistance, OldPhysicsResistance);
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
	if (EffectProperties.TargetCharacter->Implements<UCombatInterface>() && ICombatInterface::Execute_bIsDead(EffectProperties.TargetCharacter)) return;

	if (data.EvaluatedData.Attribute == GetHealthAttribute()) {
		SetHealth(FMath::Clamp(GetHealth(), 0.f, GetMaxHealth()));
	}

	if (data.EvaluatedData.Attribute == GetManaAttribute()) {
		SetMana(FMath::Clamp(GetMana(), 0.f, GetMaxMana()));
	}

	if (data.EvaluatedData.Attribute == GetIncoming_DamageAttribute()) {
		HandleIncomingDamage(EffectProperties);
	}
	if (data.EvaluatedData.Attribute == GetIncoming_ExpAttribute()) {
		HandleIncomingXP(EffectProperties);
	}
}

void UAuraAttributeSet::PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue)
{
	Super::PostAttributeChange(Attribute, OldValue, NewValue);

	if (Attribute == GetMaxHealthAttribute() && bTopOffHealth) {
		bTopOffHealth = false;
		SetHealth(GetMaxHealth());
	}
	if (Attribute == GetMaxManaAttribute() && bTopOffMana) {
		bTopOffMana = false;
		SetMana(GetMaxMana());
	}

}

void UAuraAttributeSet::ShowFloatingText(const FEffectProperties& Props, float Damage, bool bBlockedHit, bool bCriticalHit) const
{
	if (Props.SourceCharacter != Props.TargetCharacter) {
		if (AAuraPlayerController* PC = Cast<AAuraPlayerController>
			(Props.SourceCharacter->Controller)) {
			PC->ShowDamageNumber(Damage, Props.TargetCharacter, bBlockedHit, bCriticalHit);
			return;
		}
		if (AAuraPlayerController* PC = Cast<AAuraPlayerController>
			(Props.TargetCharacter->Controller)) {
			PC->ShowDamageNumber(Damage, Props.TargetCharacter, bBlockedHit, bCriticalHit);
		}
	}
}

void UAuraAttributeSet::SendEXPEvent(const FEffectProperties& Props)
{
	if (!Props.TargetCharacter->Implements<UCombatInterface>()) return;

	const int32 level = ICombatInterface::Execute_GetPlayerLevel(Props.TargetCharacter);
	const ECharacterClass CharacterClass = ICombatInterface::Execute_GetCharacterClass(Props.TargetCharacter);

	int32 RewardEXP =
		UAuraAbilitySystemLibrary::GetRewardExpForClassAndLevel(
			Props.TargetCharacter,
			CharacterClass,
			level
		);
	const FAuraGameplayTags AuraTags = FAuraGameplayTags::Get();
	FGameplayEventData Payload;
	Payload.EventTag = AuraTags.Attributes_Meta_IncomingEXP;
	Payload.EventMagnitude = RewardEXP;

	FGameplayTag EventTag = AuraTags.Attributes_Meta_IncomingEXP;

	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(Props.SourceCharacter, EventTag,Payload);
}

void UAuraAttributeSet::HandleIncomingDamage(const FEffectProperties& EffectProperties)
{
	const float LocalIncomingDamage = GetIncoming_Damage();
	SetIncoming_Damage(0.f);
	if (LocalIncomingDamage > 0.f) {
		const float NewHealth = GetHealth() - LocalIncomingDamage;
		SetHealth(FMath::Clamp(NewHealth, 0.f, GetMaxHealth()));

		const bool bFatal = NewHealth <= 0.f;
		if (!bFatal) {
			if (EffectProperties.TargetCharacter->Implements<UCombatInterface>() && !ICombatInterface::Execute_IsBeingShocked(EffectProperties.TargetCharacter)) {
				FGameplayTagContainer TagContainer;
				TagContainer.AddTag(FAuraGameplayTags::Get().Effects_HitReact);
				EffectProperties.ASCTarget->TryActivateAbilitiesByTag(TagContainer);
			}
			
			const FVector& force = UAuraAbilitySystemLibrary::GetKnockbackForce(EffectProperties.EffectContextHandle);
			if (!force.IsNearlyZero(1.f)) {
				EffectProperties.TargetCharacter->LaunchCharacter(force, true, true);
			}
			
		}
		else {
			ICombatInterface* CombatInterface = Cast<ICombatInterface>(EffectProperties.TargetAvatarActor);
			if (CombatInterface) {
				
				CombatInterface->Die(UAuraAbilitySystemLibrary::GetDeathImpulse(EffectProperties.EffectContextHandle));
			}
			SendEXPEvent(EffectProperties);
		}
		//Call the damage text widget to it.

		const bool bIsBlockHit = UAuraAbilitySystemLibrary::IsBlockHit(EffectProperties.EffectContextHandle);
		const bool bIsCriticalHit = UAuraAbilitySystemLibrary::IsCriticalHit(EffectProperties.EffectContextHandle);
		ShowFloatingText(EffectProperties, LocalIncomingDamage, bIsBlockHit, bIsCriticalHit);

		if (UAuraAbilitySystemLibrary::IsSuccessfulDebuff(EffectProperties.EffectContextHandle)) {
			Debuff(EffectProperties);
		}
	}
}

void UAuraAttributeSet::HandleIncomingXP(const FEffectProperties& EffectProperties)
{
	float Incoming_exp = GetIncoming_Exp();
	SetIncoming_Exp(0.f);

	if (EffectProperties.SourceCharacter->Implements<UPlayerInterface>()
		&& EffectProperties.SourceCharacter->Implements<UCombatInterface>()) {
		const int32 CurrentLevel = ICombatInterface::Execute_GetPlayerLevel(EffectProperties.SourceCharacter);
		const int32 CurrentExp = IPlayerInterface::Execute_GetExp(EffectProperties.SourceCharacter);

		const int32 NewLevel = IPlayerInterface::Execute_FindLevelFromAmountOfExp(EffectProperties.SourceCharacter, CurrentExp + Incoming_exp);
		int32 DeltaLevel = NewLevel - CurrentLevel;
		if (DeltaLevel > 0) {
			int32 AttributePoints = 0;
			int32 SpellPoints = 0;
			for (int i = CurrentLevel; i < NewLevel;++i) {
				AttributePoints += IPlayerInterface::Execute_GetAttributePointsReward(EffectProperties.SourceCharacter, i);
				SpellPoints += IPlayerInterface::Execute_GetSpellPointsReward(EffectProperties.SourceCharacter, i);
			}
			IPlayerInterface::Execute_AddToPlayerLevel(EffectProperties.SourceCharacter, DeltaLevel);
			IPlayerInterface::Execute_AddToAttributePoints(EffectProperties.SourceCharacter, AttributePoints);
			IPlayerInterface::Execute_AddToSpellPoints(EffectProperties.SourceCharacter, SpellPoints);

			bTopOffHealth = true;
			bTopOffMana = true;

			IPlayerInterface::Execute_LevelUp(EffectProperties.SourceCharacter);
		}
		IPlayerInterface::Execute_AddToExp(EffectProperties.SourceCharacter, Incoming_exp);
	}
}

void UAuraAttributeSet::Debuff(const FEffectProperties& Props)
{
	const FAuraGameplayTags& Tags = FAuraGameplayTags::Get();
	FGameplayEffectContextHandle EffectContext = Props.ASCSource->MakeEffectContext();
	EffectContext.AddSourceObject(Props.SourceAvatarActor);

	// Get damage type,duration,frequency,damage.
	const FGameplayTag DamageType = UAuraAbilitySystemLibrary::GetDamageType(Props.EffectContextHandle);
	const float Damage = UAuraAbilitySystemLibrary::GetDebuffDamage(Props.EffectContextHandle);
	const float Duration = UAuraAbilitySystemLibrary::GetDebuffDuration(Props.EffectContextHandle);
	const float Frequency = UAuraAbilitySystemLibrary::GetDebuffFrequency(Props.EffectContextHandle);

	//Set the GameplayEffect
	FString DebuffName = FString::Printf(TEXT("DynamicDebuff_%s"), *DamageType.ToString());
	UGameplayEffect* Effect = NewObject<UGameplayEffect>(GetTransientPackage(), FName(DebuffName));

	//Set the policy for effect
	Effect->DurationPolicy = EGameplayEffectDurationType::HasDuration;
	Effect->Period = Frequency;
	Effect->DurationMagnitude = FScalableFloat(Duration);

	//Grant Tag -> convenient to play the animation by this tag.
	//Effect->InheritableOwnedTagsContainer.AddTag(Tags.DamageTypesToDebuffs[DamageType]);
	const FGameplayTag DebuffTag = Tags.DamageTypesToDebuffs[DamageType];
	FInheritedTagContainer TagContainer = FInheritedTagContainer();
	UTargetTagsGameplayEffectComponent& Component = Effect->FindOrAddComponent<UTargetTagsGameplayEffectComponent>();
	TagContainer.Added.AddTag(DebuffTag);
	if (DebuffTag.MatchesTagExact(Tags.Debuff_Stun))
	{
		// Stunned, so block input
		TagContainer.Added.AddTag(Tags.Player_Block_CursorTrace);
		TagContainer.Added.AddTag(Tags.Player_Block_InputHeld);
		TagContainer.Added.AddTag(Tags.Player_Block_InputPressed);
		TagContainer.Added.AddTag(Tags.Player_Block_InputReleased);
	}
	Component.SetAndApplyTargetTagChanges(TagContainer);

	//Set the stacking
	Effect->StackingType = EGameplayEffectStackingType::AggregateBySource;
	Effect->StackLimitCount = 1;

	//Set the Modifier for effect
	const int index = Effect->Modifiers.Num();
	Effect->Modifiers.Add(FGameplayModifierInfo());
	FGameplayModifierInfo& ModifierInfo = Effect->Modifiers[index];

	ModifierInfo.ModifierMagnitude = FScalableFloat(Damage);
	ModifierInfo.ModifierOp = EGameplayModOp::Additive;
	ModifierInfo.Attribute = UAuraAttributeSet::GetIncoming_DamageAttribute();

	if (FGameplayEffectSpec* MutableEffectSpec = new FGameplayEffectSpec(Effect, EffectContext, 1.f)) {
		FAuraGameplayEffectContext* AuraContext = static_cast<FAuraGameplayEffectContext*>(MutableEffectSpec->GetContext().Get());
		TSharedPtr<FGameplayTag> DebuffDamageType = MakeShareable(new FGameplayTag(DamageType));
		AuraContext->SetDamageType(DebuffDamageType);

		Props.ASCTarget->ApplyGameplayEffectSpecToSelf(*MutableEffectSpec);
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



