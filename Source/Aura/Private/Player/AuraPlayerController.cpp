// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/AuraPlayerController.h"

#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "Input/AuraInputComponent.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "Components/SplineComponent.h"
#include "AuraGameplayTags.h"
#include "NavigationPath.h"
#include "NavigationSystem.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "UI/Widget/DamageWidgetComponent.h"
#include "GameFramework/Character.h"

AAuraPlayerController::AAuraPlayerController()
{
	bReplicates = true;

	Spline = CreateDefaultSubobject<USplineComponent>("Spline");
}

void AAuraPlayerController::PlayerTick(float DeltaTime)
{
	Super::PlayerTick(DeltaTime);

	CursorTrace();
	
	AutoRun();
}

void AAuraPlayerController::ShowDamageNumber_Implementation(float DamageAmount, ACharacter* TargetCharacter, bool bBlockedHit, bool bCriticalHit)
{
	if (IsValid(TargetCharacter) && DamageWidgetCompClass && IsLocalController())
	{
		UDamageWidgetComponent* DamageText = NewObject<UDamageWidgetComponent>(TargetCharacter, DamageWidgetCompClass);
		
		DamageText->RegisterComponent();
		DamageText->AttachToComponent(TargetCharacter->GetRootComponent(), FAttachmentTransformRules::KeepRelativeTransform);
		DamageText->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
		DamageText->SetDamageText(DamageAmount, bBlockedHit, bCriticalHit);
		
	}
}

void AAuraPlayerController::AutoRun()
{
	if (!bAutoRunning) return;

	if (APawn* ControllerRef = GetPawn()) {
		const FVector LocationSpline = Spline->FindLocationClosestToWorldLocation(ControllerRef->GetActorLocation(),
			ESplineCoordinateSpace::World);
		const FVector Direction = Spline->FindDirectionClosestToWorldLocation(LocationSpline,
			ESplineCoordinateSpace::World);
		ControllerRef->AddMovementInput(Direction);

		const float Distance = (CachedDestination - LocationSpline).Length();
		if (Distance <= AutoRunningAcceptRadius) {
			bAutoRunning = false;
		}
	}
}


void AAuraPlayerController::BeginPlay()
{
	Super::BeginPlay();
	check(AuraContext)

	UEnhancedInputLocalPlayerSubsystem* subsystem =
		ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer());
	if (subsystem) {
		subsystem->AddMappingContext(AuraContext, 0);
	}


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

	UAuraInputComponent* AuraInputComp = CastChecked<UAuraInputComponent>(InputComponent);
	AuraInputComp->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AAuraPlayerController::Move);
	AuraInputComp->BindAction(ShiftAction, ETriggerEvent::Started, this, &AAuraPlayerController::ShiftPress);
	AuraInputComp->BindAction(ShiftAction, ETriggerEvent::Completed, this, &AAuraPlayerController::ShiftRelease);
	AuraInputComp->BindAbilityActions(InputConfig, this, 
		             &AAuraPlayerController::AbilityTagPress, 
					 &AAuraPlayerController::AbilityTagRelease,
		             &AAuraPlayerController::AbilityTagHeld);
}

void AAuraPlayerController::AbilityTagPress(FGameplayTag GameplayTag)
{
	if (GameplayTag.MatchesTagExact(FAuraGameplayTags::Get().InputTag_LMB)) {
		bIsTargeting = ThisActor? true : false;
		bAutoRunning = false;
	}
}

void AAuraPlayerController::AbilityTagRelease(FGameplayTag GameplayTag)
{
	if (GameplayTag.MatchesTagExact(FAuraGameplayTags::Get().InputTag_LMB)) {
		if (GetASC()) {
			GetASC()->AbilityTagRelease(GameplayTag);
		}
	}
	if (GetASC()) GetASC()->AbilityTagRelease(GameplayTag);

	if (!bIsTargeting && !bIsShiftPress) {
		APawn* ControllerRef = GetPawn();
		if (FollowTime <= ShortPressThreshold && ControllerRef) {
			FVector PathStart = ControllerRef->GetActorLocation();
			if (UNavigationPath* Path =
				UNavigationSystemV1::FindPathToLocationSynchronously(this, PathStart, CachedDestination)) {

				Spline->ClearSplinePoints();
				for (const FVector& point : Path->PathPoints) {
					Spline->AddSplinePoint(point, ESplineCoordinateSpace::World);
				}
				if (Path->PathPoints.Num() > 0)
				{
					CachedDestination = Path->PathPoints[Path->PathPoints.Num() - 1];
					bAutoRunning = true;
				}
			}
		}
		FollowTime = 0.f;
		bIsTargeting = false;
	}
}

void AAuraPlayerController::AbilityTagHeld(FGameplayTag GameplayTag)
{
	if (GameplayTag.MatchesTagExact(FAuraGameplayTags::Get().InputTag_LMB)) {
		if (GetASC()) {
			GetASC()->AbilityTagHeld(GameplayTag);
		}
	}
	if (bIsTargeting || bIsShiftPress) {
		if (GetASC()) {
			GetASC()->AbilityTagHeld(GameplayTag);
		}
	}
	else {
		FollowTime += GetWorld()->GetDeltaSeconds();

		if (CursorHit.bBlockingHit) {
			CachedDestination = CursorHit.ImpactPoint;
		}

		if (APawn* ControllerRef = GetPawn()) {
			FVector WorldDirection = (CachedDestination - ControllerRef->GetActorLocation()).GetSafeNormal();
			ControllerRef->AddMovementInput(WorldDirection);
		}
	}
}

UAuraAbilitySystemComponent* AAuraPlayerController::GetASC()
{
	if (AuraAbilitySystemComp == nullptr) {
		AuraAbilitySystemComp = Cast<UAuraAbilitySystemComponent>
			(UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent((GetPawn<APawn>())
		));
	}
	return AuraAbilitySystemComp;
}

void AAuraPlayerController::ShiftPress()
{
	bIsShiftPress = true;
}

void AAuraPlayerController::ShiftRelease()
{
	bIsShiftPress = false;
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

void AAuraPlayerController::CursorTrace()
{
	GetHitResultUnderCursor(ECC_Visibility, false, CursorHit);
	if (!CursorHit.bBlockingHit) return;
	LastActor = ThisActor;
	ThisActor = CursorHit.GetActor();
	if (LastActor == nullptr)
	{
		if (ThisActor != nullptr)
		{
			ThisActor->HighLight();
		}
	}
	else
	{
		if (ThisActor == nullptr)
		{
			LastActor->UnHighLight();
		}
		else
		{
			if (LastActor != ThisActor)
			{
				LastActor->UnHighLight();
				ThisActor->HighLight();
			}
		}
	}
}
