// Fill out your copyright notice in the Description page of Project Settings.


#include "Input/InputActionDataAsset.h"

const UInputAction* UInputActionDataAsset::FindAbilityInputActionInfo(const FGameplayTag& GameplayTag, bool bLogFound)
{
	for (const FAuraInputAction& i : AbilityInputAction) {
		if (i.GameplayTag == GameplayTag) {
			return i.AbilityInputAction;
		}

		if (bLogFound) {
			UE_LOG(LogTemp, Warning, TEXT("Can't find the tag : %s , emerge on the file : %s"),
				*GameplayTag.ToString(), *GetNameSafe(this));
		}
	}

	return nullptr;
}
