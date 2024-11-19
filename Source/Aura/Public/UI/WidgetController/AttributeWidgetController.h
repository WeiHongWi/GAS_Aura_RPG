// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/WidgetController/AuraWidgetController.h"
#include "AttributeWidgetController.generated.h"


struct FAuraAttributesInfo;
class UAttributeInfo;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(
	FAttributeInfoSignature,
	const FAuraAttributesInfo& ,info
);


/**
 * 
 */
UCLASS(BlueprintType, Blueprintable)
class AURA_API UAttributeWidgetController : public UAuraWidgetController
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable)
	virtual void BroadcastInitValue() override;

	UFUNCTION(BlueprintCallable)
	virtual void BindCallbacksToDependencies() override;

	UPROPERTY(BlueprintAssignable, Category = "GAS|Attribute")
	FAttributeInfoSignature AttributeInfoDelegate;

protected:
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly)
	TObjectPtr<UAttributeInfo> AttributeInfo;
};
