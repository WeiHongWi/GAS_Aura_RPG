// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MVVMViewModelBase.h"
#include "MVVM_LoadScreen.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FLoadScreenButtonEnable);
class UMVVM_LoadSlot;
/**
 * 
 */
UCLASS()
class AURA_API UMVVM_LoadScreen : public UMVVMViewModelBase
{
	GENERATED_BODY()

public:
	void InitializeSlot();
	
	UPROPERTY(BlueprintAssignable)
	FLoadScreenButtonEnable LoadScreenButtonEnable;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UMVVM_LoadSlot> LoadSlotViewModelClass;

	UFUNCTION(BlueprintPure)
	UMVVM_LoadSlot* GetSlotByIndex(int32 index) const;

	UFUNCTION(BlueprintCallable)
	void NewGameButtonPressed(int32 index);

	UFUNCTION(BlueprintCallable)
	void NewSlotButtonPressed(int32 index,const FString& EnterName);

	UFUNCTION(BlueprintCallable)
	void SelectSlotButtonPressed(int32 index);
	void LoadData();

	UFUNCTION(BlueprintCallable)
	void DeleteButtonPressed();

	UFUNCTION(BlueprintCallable)
	void PlayButtonPressed();

	//Field Notify
	void SetNumOfSlot(int32 number);

	const int32& GetNumOfSlot() const { return NumOfSlot; }
private:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, FieldNotify, Setter, Getter, meta = (AllowPrivateAccess = "true"));
	int32 NumOfSlot;

	UPROPERTY()
	TMap<int32, UMVVM_LoadSlot*> LoadSlotMap;

	UPROPERTY()
	TObjectPtr<UMVVM_LoadSlot> LoadSlot_0;

	UPROPERTY()
	TObjectPtr<UMVVM_LoadSlot> LoadSlot_1;

	UPROPERTY()
	TObjectPtr<UMVVM_LoadSlot> LoadSlot_2;

	UPROPERTY()
	TObjectPtr<UMVVM_LoadSlot> SelectedSlot;
};
