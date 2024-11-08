// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/AuraPlayerController.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"

AAuraPlayerController::AAuraPlayerController()
{
	bReplicates = true;
}



void AAuraPlayerController::BeginPlay()
{
	Super::BeginPlay();
	check(AuraContext)

	UEnhancedInputLocalPlayerSubsystem* subsystem =
		ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer());
	check(subsystem);

	subsystem->AddMappingContext(AuraContext, 0);

	bShowMouseCursor = true;
	DefaultMouseCursor = EMouseCursor::Default;

	FInputModeGameAndUI InputModeData;
	InputModeData.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
	InputModeData.SetHideCursorDuringCapture(false);
	SetInputMode(InputModeData);

}

void AAuraPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	UEnhancedInputComponent* EnhancedInputComp = CastChecked<UEnhancedInputComponent>(InputComponent);
	EnhancedInputComp
		->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AAuraPlayerController::Move);

}

void AAuraPlayerController::Move(const FInputActionValue& ActionValue)
{
	const FVector2D value = ActionValue.Get<FVector2D>();
	const FRotator rotate = GetControlRotation();
	const FRotator rotate_yaw(0, rotate.Yaw, 0);

	const FVector Forward = FRotationMatrix(rotate_yaw).GetUnitAxis(EAxis::X);
	const FVector Right = FRotationMatrix(rotate_yaw).GetUnitAxis(EAxis::Y);

	if (APawn* PawnRef = GetPawn<APawn>()) {
		PawnRef->AddMovementInput(Forward, value.Y);
		PawnRef->AddMovementInput(Right, value.X);
	}
	

}