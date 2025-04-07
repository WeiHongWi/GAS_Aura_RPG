// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MVVMViewModelBase.h"
#include "Game/LoadScreenSaveGame.h"
#include "MVVM_LoadSlot.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FSetWidgetSwitcherIndex, int32, WidgetSwitchIndex);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FSelectButtonEnable, bool, bEnable);

/**
 * 
 */
UCLASS()
class AURA_API UMVVM_LoadSlot : public UMVVMViewModelBase
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintAssignable)
	FSetWidgetSwitcherIndex SetWidgetSwitcherIndex;

	UPROPERTY(BlueprintAssignable)
	FSelectButtonEnable SelectButtonEnable;

	void InitializeSlot();

	UPROPERTY()
	int32 SlotIndex;

	UPROPERTY()
	TEnumAsByte<SaveSlotStatus> SlotStatus;

	UPROPERTY()
	FName PlayerStartTag;


	// Field Notify

	//Setter
	void SetLocalSlotName(FString SlotName);
	void SetPlayerName(FString Name);
	void SetMapName(FString Name);
	void SetLevel(int32 level);
	//Getter
	const FString& GetLocalSlotName() const { return LocalSlotName; }
	const FString& GetPlayerName() const { return PlayerName; }
	const FString& GetMapName() const { return MapName; }
	const int32& GetLevel() const { return Level; }

private:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, FieldNotify, Setter, Getter, meta = (AllowPrivateAccess = "true"));
	FString LocalSlotName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, FieldNotify, Setter, Getter, meta = (AllowPrivateAccess = "true"));
	FString PlayerName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, FieldNotify, Setter, Getter, meta = (AllowPrivateAccess = "true"));
	FString MapName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, FieldNotify, Setter, Getter, meta = (AllowPrivateAccess = "true"));
	int32 Level;
};
