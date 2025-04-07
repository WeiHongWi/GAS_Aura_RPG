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
#include "NiagaraFunctionLibrary.h"
#include "Actor/MagicCircleActor.h"
#include "Components/DecalComponent.h"
#include "Aura/Aura.h"

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

	UpdateMagicCircleLocation();
}

void AAuraPlayerController::ShowMagicCircle(UMaterialInterface* DecalMaterial)
{
	if (!IsValid(MagicCircleComp)) {
		MagicCircleComp = GetWorld()->SpawnActor<AMagicCircleActor>(MagicCircleCompClass);
		if (DecalMaterial) {
			MagicCircleComp->MagicCircleDecal->SetMaterial(0, DecalMaterial);
		}
	}
}

void AAuraPlayerController::HideMagicCircle()
{
	if (IsValid(MagicCircleComp)) {
		MagicCircleComp->Destroy();
	}
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
		const FVector LocationSpline = Spline->FindLocationClosestToWorldLocation(
			ControllerRef->GetActorLocation(),
			ESplineCoordinateSpace::World
		);
		const FVector Direction = Spline->FindDirectionClosestToWorldLocation(
			LocationSpline,
			ESplineCoordinateSpace::World
		);
		ControllerRef->AddMovementInput(Direction);

		const float Distance = (LocationSpline - CachedDestination).Length();
		if (Distance <= AutoRunningAcceptRadius) {
			bAutoRunning = false;
		}
	}
}

void AAuraPlayerController::UpdateMagicCircleLocation()
{
	if (IsValid(MagicCircleComp)) {
		MagicCircleComp->SetActorLocation(CursorHit.ImpactPoint);
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
	if (GetASC() && GetASC()->HasMatchingGameplayTag(FAuraGameplayTags::Get().Player_Block_InputPressed)) {
		return;
	}
	
	if (GameplayTag.MatchesTagExact(FAuraGameplayTags::Get().InputTag_LMB)) {
		bIsTargeting = ThisActor? true : false;
		bAutoRunning = false;
	}

	if (GetASC()) GetASC()->AbilityTagPress(GameplayTag);
}

void AAuraPlayerController::AbilityTagRelease(FGameplayTag GameplayTag)
{
	if (GetASC() && GetASC()->HasMatchingGameplayTag(FAuraGameplayTags::Get().Player_Block_InputReleased)) {
		return;
	}
	
	if (!GameplayTag.MatchesTagExact(FAuraGameplayTags::Get().InputTag_LMB)) {
		if (GetASC()) {
			GetASC()->AbilityTagRelease(GameplayTag);
			return;
		}
	}
	if (GetASC()) GetASC()->AbilityTagRelease(GameplayTag);

	if (!bIsShiftPress && !bIsTargeting) {
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
			if (GetASC() && !GetASC()->HasMatchingGameplayTag(FAuraGameplayTags::Get().Player_Block_InputPressed)) {
				UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, ClickNiagaraSystem, CachedDestination);
			}
		}
		FollowTime = 0.f;
		bIsTargeting = false;
	}
}

void AAuraPlayerController::AbilityTagHeld(FGameplayTag GameplayTag)
{
	if (GetASC() && GetASC()->HasMatchingGameplayTag(FAuraGameplayTags::Get().Player_Block_InputHeld)) {
		return;
	}
	if (!GameplayTag.MatchesTagExact(FAuraGameplayTags::Get().InputTag_LMB)) {
		if (GetASC()) {
			GetASC()->AbilityTagHeld(GameplayTag);
			return;
		}
	}
	if (bIsTargeting || bIsShiftPress) {
		if (GetASC()) {
			GetASC()->AbilityTagHeld(GameplayTag);
		}
	}
	else {
		FollowTime += GetWorld()->GetDeltaSeconds();

		FHitResult Hit;
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
	if (GetASC() && GetASC()->HasMatchingGameplayTag(FAuraGameplayTags::Get().Player_Block_CursorTrace)) {
		if(LastActor)LastActor->UnHighLight();
		if(ThisActor)ThisActor->UnHighLight();
		LastActor = nullptr;
		ThisActor = nullptr;
		return;
	}
	const ECollisionChannel channel = (!IsValid(MagicCircleComp)) ? ECC_Visibility : ECC_ExcludePlayer;
	GetHitResultUnderCursor(channel, false, CursorHit);
	
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
