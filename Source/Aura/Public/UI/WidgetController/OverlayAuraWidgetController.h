// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/WidgetController/AuraWidgetController.h"
#include "OverlayAuraWidgetController.generated.h"

/**
 * 
 */
struct FAuraAbilityInfo;
class UAuraUserWidget;
class UAbilityInfo;
class UAuraAbilitySystemComponent;

USTRUCT(BlueprintType)
struct FUIWidgetRow : public FTableRowBase {
	GENERATED_BODY();

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FGameplayTag MessageTag = FGameplayTag();

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FText Message = FText();

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSubclassOf<class UAuraUserWidget> MessageWidget;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UTexture2D* Image = nullptr;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(
	FOnAttributeChangeSignature,
	float, value
);


DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(
	FMessageWidgetSignature,
	FUIWidgetRow,Row
);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(
	FAbilityInfoSignature
	,const FAuraAbilityInfo&,
	Info
);




UCLASS(BlueprintType,Blueprintable)
class AURA_API UOverlayAuraWidgetController : public UAuraWidgetController
{
	GENERATED_BODY()
	
public:
	virtual void BroadcastInitValue() override;
	virtual void BindCallbacksToDependencies() override;

	UPROPERTY(BlueprintAssignable, Category = "GAS|Attribute")
	FOnAttributeChangeSignature OnHealthChange;

	UPROPERTY(BlueprintAssignable, Category = "GAS|Attribute")
	FOnAttributeChangeSignature OnManaChange;

	UPROPERTY(BlueprintAssignable, Category = "GAS|Attribute")
	FOnAttributeChangeSignature OnMaxHealthChange;

	UPROPERTY(BlueprintAssignable, Category = "GAS|Attribute")
	FOnAttributeChangeSignature OnMaxManaChange;

	UPROPERTY(BlueprintAssignable, Category = "GAS|Messages")
	FMessageWidgetSignature MessageWidgetDelegate;

	UPROPERTY(BlueprintAssignable, Category = "GAS|Messages")
	FAbilityInfoSignature AbilityInfoDelegate;

	UPROPERTY(BlueprintAssignable, Category = "GAS|Exp")
	FOnAttributeChangeSignature OnXpPercentChangeDelegate;
protected:

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Widget Value")
	TObjectPtr<UDataTable> WidgetDT;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ability Info")
	TObjectPtr<UAbilityInfo> AbilityInfo;

	template<typename T>
	T* GetDatatableRowByTag(TObjectPtr<UDataTable> datatable, const FGameplayTag& Tag);

	void OnInitializeStartupAbilities(UAuraAbilitySystemComponent* AuraAbilitySystemComponent);

	void OnExpChanged(int32 exp);
};

template<typename T>
T* UOverlayAuraWidgetController::GetDatatableRowByTag(TObjectPtr<UDataTable> datatable, const FGameplayTag& Tag)
{
	return datatable->FindRow<T>(Tag.GetTagName(),TEXT(""));
}
