// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Data/AbilityInfo.h"

FAuraAbilityInfo UAbilityInfo::GetAbilityInfoByTag(const FGameplayTag& tag, bool bLog) const
{
	for (const FAuraAbilityInfo& info : AbilityInformation) {
		if (info.AbilityTag.MatchesTagExact(tag)) {
			return info;
		}
	}
	if (bLog) {
		UE_LOG(LogTemp, Warning,
			TEXT("The tag is not found : %s , is in file :%s"),
			*tag.ToString(),
			*GetNameSafe(this)
		)
	}

	return FAuraAbilityInfo();
}
