// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/MMC/MMC_Mana.h"

#include "AbilitySystem/AuraAttributeSet.h"
#include "Interaction/CombatInterface.h"

UMMC_Mana::UMMC_Mana()
{
	IntelligenceDef.AttributeToCapture = UAuraAttributeSet::GetIntelligenceAttribute();
	IntelligenceDef.AttributeSource = EGameplayEffectAttributeCaptureSource::Target;
	IntelligenceDef.bSnapshot = false;

	RelevantAttributesToCapture.Add(IntelligenceDef);
}

float UMMC_Mana::CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const
{
	const FGameplayTagContainer* SrcTag = Spec.CapturedSourceTags.GetAggregatedTags();
	const FGameplayTagContainer* TarTag = Spec.CapturedTargetTags.GetAggregatedTags();
	
	FAggregatorEvaluateParameters EvaluateParams;
	EvaluateParams.SourceTags = SrcTag;
	EvaluateParams.TargetTags = TarTag;
	
	float Intelligence = 0.f;
	GetCapturedAttributeMagnitude(IntelligenceDef, Spec, EvaluateParams, Intelligence);
	Intelligence = FMath::Max<float>(Intelligence, 0.f);

	int32 PlayerLevel = 1;
	if (Spec.GetContext().GetSourceObject()->Implements<UCombatInterface>())
	{
		PlayerLevel = ICombatInterface::Execute_GetPlayerLevel(Spec.GetContext().GetSourceObject());
	}

	return 80.f + 2.f * Intelligence + 10.f * PlayerLevel;
}
