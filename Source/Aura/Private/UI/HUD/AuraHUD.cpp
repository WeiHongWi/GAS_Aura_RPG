// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/HUD/AuraHUD.h"
#include "UnrealWidgetFwd.h"
#include "UI/Widget/AuraUserWidget.h"
#include "UI/WidgetController/AttributeWidgetController.h"
#include "UI/WidgetController/OverlayAuraWidgetController.h"

UOverlayAuraWidgetController* AAuraHUD::GetOverlayWidgetController(const FWidgetControllerParams& WCParams)
{
	if (OverlayWidgetController == nullptr) {
		OverlayWidgetController = NewObject<UOverlayAuraWidgetController>(this,OverlayWidgetControllerClass);
		OverlayWidgetController->SetWidgetController(WCParams);
		OverlayWidgetController->BindCallbacksToDependencies();

		return OverlayWidgetController;
	}

	return OverlayWidgetController;
}

UAttributeWidgetController* AAuraHUD::GetAttributeMenuWidgetController(const FWidgetControllerParams& WCParams)
{
	if (AttributeWidgetController == nullptr) {
		AttributeWidgetController = NewObject<UAttributeWidgetController>(this, AttributeWidgetControllerClass);
		AttributeWidgetController->SetWidgetController(WCParams);
		AttributeWidgetController->BindCallbacksToDependencies();

		return AttributeWidgetController;
	}

	return AttributeWidgetController;
}
void AAuraHUD::InitOverlay(APlayerState* PS, APlayerController* PC, UAbilitySystemComponent* ASC, UAttributeSet* AS)
{
	checkf(OverlayWidgetClass, TEXT("OverlayWidgetClass is uninitialized,please check AuraHUD."));
	checkf(OverlayWidgetControllerClass, TEXT("OverlayWidgetControllerClass is uninitialized,please check AuraHUD."));
	
	UUserWidget* widget = CreateWidget<UUserWidget>(GetWorld(), OverlayWidgetClass);
	OverlayWidget = Cast<UAuraUserWidget>(widget);
	
	const struct FWidgetControllerParams WCParams(PC, PS, ASC, AS);
	UOverlayAuraWidgetController* WidgetController = GetOverlayWidgetController(WCParams);

	OverlayWidget->SetWidgerController(WidgetController);
	WidgetController->BroadcastInitValue();
	
	
	widget->AddToViewport();
}

