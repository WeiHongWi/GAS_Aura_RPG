// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "AuraHUD.generated.h"

class UAuraUserWidget;
class UOverlayAuraWidgetController;
class UAttributeWidgetController;
class USpellMenuWidgetController;
class UAbilitySystemComponent;
class UAttributeSet;
/**
 * 
 */
UCLASS()
class AURA_API AAuraHUD : public AHUD
{
	GENERATED_BODY()
	
public:
	UPROPERTY()
	TObjectPtr<UAuraUserWidget>  OverlayWidget;

	UOverlayAuraWidgetController* GetOverlayWidgetController(const struct FWidgetControllerParams& WCParams);

	UAttributeWidgetController* GetAttributeMenuWidgetController(const struct FWidgetControllerParams& WCParams);

	USpellMenuWidgetController* GetSpellMenuWidgetController(const struct FWidgetControllerParams& WCParams);
	
	void InitOverlay(APlayerState* PS, APlayerController* PC, UAbilitySystemComponent* ASC, UAttributeSet* AS);

private:
	UPROPERTY(EditAnywhere)
	TSubclassOf<UAuraUserWidget> OverlayWidgetClass;

	UPROPERTY()
	TObjectPtr<UOverlayAuraWidgetController> OverlayWidgetController;

	UPROPERTY(EditAnywhere)
	TSubclassOf<UOverlayAuraWidgetController> OverlayWidgetControllerClass;

	UPROPERTY()
	TObjectPtr<UAttributeWidgetController> AttributeWidgetController;

	UPROPERTY(EditAnywhere)
	TSubclassOf<UAttributeWidgetController> AttributeWidgetControllerClass;

	UPROPERTY()
	TObjectPtr<USpellMenuWidgetController> SpellMenuWidgetController;

	UPROPERTY(EditAnywhere)
	TSubclassOf<USpellMenuWidgetController> SpellMenuWidgetControllerClass;
};
