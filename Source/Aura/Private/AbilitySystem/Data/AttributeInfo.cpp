// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Data/AttributeInfo.h"

FAuraAttributesInfo UAttributeInfo::FindAttributeForTag(const FGameplayTag& GameplayTag, bool bLogNotFound) const
{
	for (FAuraAttributesInfo Info : AttributeInfo) {
		if (Info.AttributeTag == GameplayTag) {
			return Info;
		}

		if (bLogNotFound) {
			UE_LOG(LogTemp,Warning,
			       TEXT("The tag is not found : %s , is in file :%s"),
				   *GameplayTag.ToString(),
				   *GetNameSafe(this))
		}
	}
	
	
	return FAuraAttributesInfo();
}
