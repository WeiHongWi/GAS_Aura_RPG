// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EnhancedInputComponent.h"
#include "InputActionDataAsset.h"
#include "AuraInputComponent.generated.h"

/**
 * 
 */


UCLASS()
class AURA_API UAuraInputComponent : public UEnhancedInputComponent
{
	GENERATED_BODY()
	
public:
	template<class UserClass,typename PressedFuncType,typename ReleasedFuncType,typename HeldFuncType>
	void BindAbilityActions(const UInputActionDataAsset* InputActionConfig, UserClass* User,
		PressedFuncType PressFunc, ReleasedFuncType ReleaseFunc, HeldFuncType HeldFunc);
};



template<class UserClass, typename PressedFuncType, typename ReleasedFuncType, typename HeldFuncType>
void UAuraInputComponent::BindAbilityActions(const UInputActionDataAsset* InputActionConfig, UserClass* User, PressedFuncType PressFunc, ReleasedFuncType ReleaseFunc, HeldFuncType HeldFunc)
{
	check(InputActionConfig);
	for (const FAuraInputAction& Action : InputActionConfig->AbilityInputAction) {
		if (Action.AbilityInputAction && Action.GameplayTag.IsValid()) {
			if (PressFunc) {
				BindAction(Action.AbilityInputAction, ETriggerEvent::Started,
					User, PressFunc, Action.GameplayTag);
			}

			if (HeldFunc) {
				BindAction(Action.AbilityInputAction, ETriggerEvent::Triggered,
					User, HeldFunc, Action.GameplayTag);
			}

			if (ReleaseFunc) {
				BindAction(Action.AbilityInputAction, ETriggerEvent::Completed,
					User, ReleaseFunc, Action.GameplayTag);
			}
		}
	}
}
