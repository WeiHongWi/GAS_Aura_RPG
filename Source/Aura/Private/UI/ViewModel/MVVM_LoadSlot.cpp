// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/ViewModel/MVVM_LoadSlot.h"

void UMVVM_LoadSlot::InitializeSlot()
{
	const int32 status = SlotStatus.GetIntValue();
	SetWidgetSwitcherIndex.Broadcast(status);
}

void UMVVM_LoadSlot::SetLocalSlotName(FString SlotName)
{
	UE_MVVM_SET_PROPERTY_VALUE(LocalSlotName, SlotName);
}

void UMVVM_LoadSlot::SetPlayerName(FString Name)
{
	UE_MVVM_SET_PROPERTY_VALUE(PlayerName, Name);
}

void UMVVM_LoadSlot::SetMapName(FString Name)
{
	UE_MVVM_SET_PROPERTY_VALUE(MapName, Name);
}

void UMVVM_LoadSlot::SetLevel(int32 level)
{
	UE_MVVM_SET_PROPERTY_VALUE(Level,level);
}


