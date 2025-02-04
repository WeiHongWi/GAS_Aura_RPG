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
	
	TMap<FGameplayTag, FGameplayEffectAttributeCaptureDefinition> TagsToCaptureDef;

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
	
		const FAuraGameplayTags GameplayTags = FAuraGameplayTags::Get();
		TagsToCaptureDef.Add(GameplayTags.Attributes_Secondary_Armor,ArmorDef);
		TagsToCaptureDef.Add(GameplayTags.Attributes_Secondary_BlockChance,Block_ChanceDef);
		TagsToCaptureDef.Add(GameplayTags.Attributes_Secondary_CriticalHitChance, Critical_Hit_ChanceDef);
		TagsToCaptureDef.Add(GameplayTags.Attributes_Secondary_CriticalHitDamage, Critical_Hit_DamageDef);
		TagsToCaptureDef.Add(GameplayTags.Attributes_Secondary_CriticalHitResistance, Critical_Hit_ResistanceDef);
		TagsToCaptureDef.Add(GameplayTags.Attributes_Secondary_ArmorPenetration, Armor_PenetrationDef);

		TagsToCaptureDef.Add(GameplayTags.Attributes_Resistance_Fire, Fire_ResistanceDef);
		TagsToCaptureDef.Add(GameplayTags.Attributes_Resistance_Lightning, Lightning_ResistanceDef);
		TagsToCaptureDef.Add(GameplayTags.Attributes_Resistance_Arcane, Arcane_ResistanceDef);
		TagsToCaptureDef.Add(GameplayTags.Attributes_Resistance_Physics, Physics_ResistanceDef);
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
	for (auto& pair : FAuraGameplayTags::Get().DamageTypesToResistance) {
		const FGameplayTag DamageType = pair.Key;
		const FGameplayTag Resistance = pair.Value;
		checkf(AuraDamageStatics().TagsToCaptureDef.Contains(Resistance), 
			TEXT("TagsToCaptureDef doesn't contain Tag: [%s] in ExecCalc_Damage"), *Resistance.ToString());


		const FGameplayEffectAttributeCaptureDefinition AttributeCaptureDef 
			= AuraDamageStatics().TagsToCaptureDef[Resistance];

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
