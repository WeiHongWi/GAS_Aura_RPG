// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/ExecCalc/ExecCalc_Damage.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystem/AuraAttributeSet.h"
#include "AuraGameplayTags.h"
#include "AbilitySystem/AuraAbilitySystemLibrary.h"
#include "AbilitySystem/Data/CharacterClassInfo.h"
#include "Interaction/CombatInterface.h"

struct AuraDamageStatics
{
	DECLARE_ATTRIBUTE_CAPTUREDEF(Armor);
	DECLARE_ATTRIBUTE_CAPTUREDEF(Armor_Penetration);
	DECLARE_ATTRIBUTE_CAPTUREDEF(Block_Chance);
	DECLARE_ATTRIBUTE_CAPTUREDEF(Critical_Hit_Damage);
	DECLARE_ATTRIBUTE_CAPTUREDEF(Critical_Hit_Chance);
	DECLARE_ATTRIBUTE_CAPTUREDEF(Critical_Hit_Resistance);

	DECLARE_ATTRIBUTE_CAPTUREDEF(Fire_Resistance);
	DECLARE_ATTRIBUTE_CAPTUREDEF(Lightning_Resistance);
	DECLARE_ATTRIBUTE_CAPTUREDEF(Arcane_Resistance);
	DECLARE_ATTRIBUTE_CAPTUREDEF(Physics_Resistance);
	

	AuraDamageStatics()
	{
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, Armor, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, Block_Chance, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, Armor_Penetration, Source, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, Critical_Hit_Damage,Source,false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, Critical_Hit_Chance,Source,false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, Critical_Hit_Resistance,Target,false);

		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, Fire_Resistance, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, Lightning_Resistance, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, Arcane_Resistance, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, Physics_Resistance, Target, false);
	
	}
};
static const AuraDamageStatics& DamageStatics()
{
	static AuraDamageStatics DStatics;
	return DStatics;
}


UExecCalc_Damage::UExecCalc_Damage()
{
	RelevantAttributesToCapture.Add(DamageStatics().ArmorDef);
	RelevantAttributesToCapture.Add(DamageStatics().Block_ChanceDef);
	RelevantAttributesToCapture.Add(DamageStatics().Armor_PenetrationDef);
	RelevantAttributesToCapture.Add(DamageStatics().Critical_Hit_ChanceDef);
	RelevantAttributesToCapture.Add(DamageStatics().Critical_Hit_DamageDef);
	RelevantAttributesToCapture.Add(DamageStatics().Critical_Hit_ResistanceDef);
	RelevantAttributesToCapture.Add(DamageStatics().Fire_ResistanceDef);
	RelevantAttributesToCapture.Add(DamageStatics().Lightning_ResistanceDef);
	RelevantAttributesToCapture.Add(DamageStatics().Arcane_ResistanceDef);
	RelevantAttributesToCapture.Add(DamageStatics().Physics_ResistanceDef);
}

void UExecCalc_Damage::Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams,
	FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const
{
	TMap<FGameplayTag, FGameplayEffectAttributeCaptureDefinition> TagsToCaptureDefs;
	const FAuraGameplayTags& Tags = FAuraGameplayTags::Get();

	TagsToCaptureDefs.Add(Tags.Attributes_Secondary_Armor, DamageStatics().ArmorDef);
	TagsToCaptureDefs.Add(Tags.Attributes_Secondary_BlockChance, DamageStatics().Block_ChanceDef);
	TagsToCaptureDefs.Add(Tags.Attributes_Secondary_ArmorPenetration, DamageStatics().Armor_PenetrationDef);
	TagsToCaptureDefs.Add(Tags.Attributes_Secondary_CriticalHitChance, DamageStatics().Critical_Hit_ChanceDef);
	TagsToCaptureDefs.Add(Tags.Attributes_Secondary_CriticalHitResistance, DamageStatics().Critical_Hit_ResistanceDef);
	TagsToCaptureDefs.Add(Tags.Attributes_Secondary_CriticalHitDamage, DamageStatics().Critical_Hit_DamageDef);

	TagsToCaptureDefs.Add(Tags.Attributes_Resistance_Arcane, DamageStatics().Arcane_ResistanceDef);
	TagsToCaptureDefs.Add(Tags.Attributes_Resistance_Fire, DamageStatics().Fire_ResistanceDef);
	TagsToCaptureDefs.Add(Tags.Attributes_Resistance_Lightning, DamageStatics().Lightning_ResistanceDef);
	TagsToCaptureDefs.Add(Tags.Attributes_Resistance_Physics, DamageStatics().Physics_ResistanceDef);
	const UAbilitySystemComponent* SourceASC = ExecutionParams.GetSourceAbilitySystemComponent();
	const UAbilitySystemComponent* TargetASC = ExecutionParams.GetTargetAbilitySystemComponent();

	AActor* SourceAvatar = SourceASC ? SourceASC->GetAvatarActor() : nullptr;
	AActor* TargetAvatar = TargetASC ? TargetASC->GetAvatarActor() : nullptr;
	int32 SourcePlayerLevel = 1;
	if (SourceAvatar->Implements<UCombatInterface>())
	{
		SourcePlayerLevel = ICombatInterface::Execute_GetPlayerLevel(SourceAvatar);
	}
	int32 TargetPlayerLevel = 1;
	if (TargetAvatar->Implements<UCombatInterface>())
	{
		TargetPlayerLevel = ICombatInterface::Execute_GetPlayerLevel(TargetAvatar);
	}

	const FGameplayEffectSpec& Spec = ExecutionParams.GetOwningSpec();

	const FGameplayTagContainer* SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
	const FGameplayTagContainer* TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();
	FAggregatorEvaluateParameters EvaluationParameters;
	EvaluationParameters.SourceTags = SourceTags;
	EvaluationParameters.TargetTags = TargetTags;
	

	float Damage = 0.0f;
	// Apply Effect damage by considering debuff.
	for (auto& pair : Tags.DamageTypesToDebuffs) {
		const FGameplayTag DamageType = pair.Key;
		const FGameplayTag Debuff = pair.Value;

		const float TypeDamage = Spec.GetSetByCallerMagnitude(DamageType, false, -1.f);
		if (TypeDamage > .5f) {
			// Determine the chance to resist the debuff;
			float Debuff_Chance = Spec.GetSetByCallerMagnitude(Tags.Debuff_Chance, false, -1.f);

			float TargetTypeResistance = 0.f;
			const FGameplayTag ResistanceTag = Tags.DamageTypesToResistance[DamageType];
			ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(
				TagsToCaptureDefs[ResistanceTag],
				EvaluationParameters,
				TargetTypeResistance
			);
			TargetTypeResistance = FMath::Max<float>(0, TargetTypeResistance);

			const float Chance = Debuff_Chance * (100 - TargetTypeResistance) / 100.f;
			const bool bDebuff = FMath::RandRange(1, 100) < Chance;
			if (bDebuff) {
				FGameplayEffectContextHandle EffectContextHandle = Spec.GetContext();

				UAuraAbilitySystemLibrary::SetIsSuccessfulDebuff(EffectContextHandle, true);
				UAuraAbilitySystemLibrary::SetDamageType(EffectContextHandle, DamageType);

				float DebuffDamage = Spec.GetSetByCallerMagnitude(Tags.Debuff_Damage, false, -1.f);
				float DebuffDuration = Spec.GetSetByCallerMagnitude(Tags.Debuff_Duration, false, -1.f);
				float DebuffFrequency = Spec.GetSetByCallerMagnitude(Tags.Debuff_Frequency, false, -1.f);
				
				UAuraAbilitySystemLibrary::SetDebuffDamage(EffectContextHandle, DebuffDamage);
				UAuraAbilitySystemLibrary::SetDebuffDuration(EffectContextHandle, DebuffDuration);
				UAuraAbilitySystemLibrary::SetDebuffFrequency(EffectContextHandle, DebuffFrequency);
			
			}
		}
	}

	// Calculate damage by considering resistance.
	for (auto& pair : Tags.DamageTypesToResistance) {
		const FGameplayTag DamageType = pair.Key;
		const FGameplayTag Resistance = pair.Value;
		checkf(TagsToCaptureDefs.Contains(Resistance),
			TEXT("TagsToCaptureDef doesn't contain Tag: [%s] in ExecCalc_Damage"), *Resistance.ToString());


		const FGameplayEffectAttributeCaptureDefinition AttributeCaptureDef 
			= TagsToCaptureDefs[Resistance];

		float DamageTypeValue = Spec.GetSetByCallerMagnitude(pair.Key,false);

		float resistance = 0.f;
		ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(AttributeCaptureDef, EvaluationParameters, resistance);
		resistance = FMath::Clamp(resistance, 0.f, 100.f);
		DamageTypeValue *= (100.f - resistance) / 100.f;

		Damage += DamageTypeValue;
	}

	//Target Armor and Source Armor Penetration
	float TargetArmor = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().ArmorDef, EvaluationParameters, TargetArmor);
	TargetArmor = FMath::Max(0.f, TargetArmor);

	float SourceArmorPenetration = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().Armor_PenetrationDef, EvaluationParameters, SourceArmorPenetration);
	SourceArmorPenetration = FMath::Max(0.f, SourceArmorPenetration);

	// Damage Coefficient
	UCharacterClassInfo* CharacterClassInfo = UAuraAbilitySystemLibrary::GetCharacterClassInfo(SourceAvatar);
	FRealCurve* RealCurve = CharacterClassInfo->DamageCoefficientTable->FindCurve(FName("SourceArmorPenetration"), FString());
	float ArmorPenetrationCoefficient = RealCurve->Eval(SourcePlayerLevel);

	UCharacterClassInfo* CharacterClassInfo_T = UAuraAbilitySystemLibrary::GetCharacterClassInfo(TargetAvatar);
	FRealCurve* RealCurve_T = CharacterClassInfo_T->DamageCoefficientTable->FindCurve(FName("EffectArmor"), FString());
	float EffectArmorCoefficient = RealCurve_T->Eval(TargetPlayerLevel);

	float EffectArmor = TargetArmor *= (100 - SourceArmorPenetration* ArmorPenetrationCoefficient) / 100;
	Damage *= (100 - EffectArmor* EffectArmorCoefficient) / 100;

	// Critical Hit : Resistance -> reduce chance of critical hit.
	UCharacterClassInfo* CharacterClassInfo_TC = UAuraAbilitySystemLibrary::GetCharacterClassInfo(TargetAvatar);
	FRealCurve* RealCurve_TC = CharacterClassInfo_TC->DamageCoefficientTable->FindCurve(FName("TargetCriticalHitResistance"), FString());
	float CriticalHitResistanceCoefficient = RealCurve_TC->Eval(TargetPlayerLevel);
	float TargetCritical_Hit_Resistance = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().Critical_Hit_ResistanceDef, EvaluationParameters, TargetCritical_Hit_Resistance);
	TargetCritical_Hit_Resistance = FMath::Max(0.f, TargetCritical_Hit_Resistance);

	// Critical Hit : Chance -> Possibility to double the damage
	float TargetCritical_Hit_Chance = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().Critical_Hit_ChanceDef, EvaluationParameters, TargetCritical_Hit_Chance);
	TargetCritical_Hit_Chance = FMath::Max(0.f, TargetCritical_Hit_Chance);

	// Critical Hit : Damage-> The constant damage after double.
	float TargetCritical_Hit_Damage = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().Critical_Hit_DamageDef, EvaluationParameters, TargetCritical_Hit_Damage);
	TargetCritical_Hit_Damage = FMath::Max(0.f, TargetCritical_Hit_Damage);

	float CriticalHitProbability = TargetCritical_Hit_Chance * (100 - TargetCritical_Hit_Resistance* CriticalHitResistanceCoefficient) / 100;
	float CriticalHitChance = FMath::FRand();


	bool ShouldCriticalHit = CriticalHitChance <= (CriticalHitProbability/100);
	FGameplayEffectContextHandle EffectContextHandle = Spec.GetContext();
	UAuraAbilitySystemLibrary::SetIsCriticalHit(EffectContextHandle, ShouldCriticalHit);

	if (ShouldCriticalHit) {
		Damage *= 2;
		Damage += TargetCritical_Hit_Damage;
	}
	
	//UE_LOG(LogTemp, Warning, TEXT("%f"), Damage);

	float Block_Chance = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().Block_ChanceDef, EvaluationParameters, Block_Chance);
	Block_Chance = FMath::Max(0.f, Block_Chance);
	Block_Chance /= 100;
	float Chance = FMath::FRand();
	bool ShouldBlock = Chance <= Block_Chance;
	UAuraAbilitySystemLibrary::SetIsBlockHit(EffectContextHandle, ShouldBlock);

	if (ShouldBlock) {
		Damage /= 2;
	}
	
	const FGameplayModifierEvaluatedData EvaluatedData(
		UAuraAttributeSet::GetIncoming_DamageAttribute(), 
		EGameplayModOp::Additive, 
		Damage
	);

	OutExecutionOutput.AddOutputModifier(EvaluatedData);
}
