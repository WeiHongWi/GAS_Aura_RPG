// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/AbilityTask/TargetDataUnderMouse.h"

#include "GameFramework/PlayerController.h"
#include "AbilitySystemComponent.h"
#include "Aura/Aura.h"

UTargetDataUnderMouse* UTargetDataUnderMouse::CreateTargetDataUnderMouse(UGameplayAbility* OwningAbility)
{
	UTargetDataUnderMouse* obj = NewAbilityTask<UTargetDataUnderMouse>(OwningAbility);
	return obj;
}

void UTargetDataUnderMouse::Activate()
{
	const bool bIsLocalControlled = Ability->GetActorInfo().IsLocallyControlled();

	if (bIsLocalControlled) {
		SendMouseData();
	}
	else {
		const FGameplayAbilitySpecHandle AbilitySpecHandle = GetAbilitySpecHandle();
		const FPredictionKey PredictionKey = GetActivationPredictionKey();

		AbilitySystemComponent.Get()->AbilityTargetDataSetDelegate(AbilitySpecHandle, 
			PredictionKey).AddUObject(this, &UTargetDataUnderMouse::OnTargetDataSetCallback);
		
		//Check server receive data from client or not.
		const bool bIsReplicated = AbilitySystemComponent.Get()->CallReplicatedTargetDataDelegatesIfSet(AbilitySpecHandle, PredictionKey);
		if (!bIsReplicated) {
			SetWaitingOnRemotePlayerData();
		}
	}
	
}

void UTargetDataUnderMouse::SendMouseData()
{
	FScopedPredictionWindow ScopedWindow(AbilitySystemComponent.Get());
	
	APlayerController* PlayerRef = GetWorld()->GetFirstPlayerController();

	FHitResult Hit;
	if (PlayerRef) {
		PlayerRef->GetHitResultUnderCursor(ECC_Target, false, Hit);
	}

	FGameplayAbilityTargetDataHandle DataHandle;
	FGameplayAbilityTargetData_SingleTargetHit* Data = new FGameplayAbilityTargetData_SingleTargetHit();
	Data->HitResult = Hit;
	DataHandle.Add(Data);

	AbilitySystemComponent->ServerSetReplicatedTargetData(
		GetAbilitySpecHandle(),
		GetActivationPredictionKey(),
		DataHandle,
		FGameplayTag(),
		AbilitySystemComponent->ScopedPredictionKey
	);

	if (ShouldBroadcastAbilityTaskDelegates()) {
		ValidData.Broadcast(DataHandle);
	}
}

void UTargetDataUnderMouse::OnTargetDataSetCallback(const FGameplayAbilityTargetDataHandle& DataHandle, FGameplayTag ActivationTag)
{
	AbilitySystemComponent.Get()->ConsumeClientReplicatedTargetData(GetAbilitySpecHandle(), 
		GetActivationPredictionKey());
	if (ShouldBroadcastAbilityTaskDelegates())
	{
		ValidData.Broadcast(DataHandle);
	}
}
