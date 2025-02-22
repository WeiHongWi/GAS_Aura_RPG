// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/AuraCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "AbilitySystemComponent.h"
#include "Player/AuraPlayerState.h"
#include "Player/AuraPlayerController.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "AbilitySystem/Data/LevelupInfo.h"
#include "NiagaraComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "UI/HUD/AuraHUD.h"



AAuraCharacter::AAuraCharacter()
{
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0, 400.f, 0);
	GetCharacterMovement()->bConstrainToPlane = true;
	GetCharacterMovement()->bSnapToPlaneAtStart = true;

	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;
	bUseControllerRotationYaw = false;

	CameraBoom = CreateDefaultSubobject<USpringArmComponent>("CameraBoom");
	CameraBoom->SetupAttachment(GetRootComponent());
	CameraBoom->SetUsingAbsoluteRotation(true);
	CameraBoom->bDoCollisionTest = false;

	TopDownCameraComponent = CreateDefaultSubobject<UCameraComponent>("TopDownCameraComponent");
	TopDownCameraComponent->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	TopDownCameraComponent->bUsePawnControlRotation = false;

	LevelupParticleComp = CreateDefaultSubobject<UNiagaraComponent>("LevelupParticleComp");
	LevelupParticleComp->SetupAttachment(GetRootComponent());
	LevelupParticleComp->bAutoActivate = false;
}

void AAuraCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	InitActorInfo();
	AddCharacterAbilities();
}

void AAuraCharacter::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();
	InitActorInfo();
}

int32 AAuraCharacter::GetExp_Implementation() const
{
	AAuraPlayerState* AuraPlayerState = GetPlayerState<AAuraPlayerState>();
	check(AuraPlayerState);

	return AuraPlayerState->GetXP();
}

void AAuraCharacter::AddToExp_Implementation(int32 exp)
{
	AAuraPlayerState* AuraPlayerState = GetPlayerState<AAuraPlayerState>();
	check(AuraPlayerState);

	AuraPlayerState->AddXP(exp);
}

void AAuraCharacter::LevelUp_Implementation()
{
	MulticastLevelupParticle();
}

void AAuraCharacter::MulticastLevelupParticle_Implementation() const
{
	if (IsValid(LevelupParticleComp)) {
		const FVector CameraLocation = TopDownCameraComponent->GetComponentLocation();
		const FVector RootLocation = LevelupParticleComp->GetComponentLocation();

		const FRotator CameraRotate = (CameraLocation - RootLocation).Rotation();
		LevelupParticleComp->SetWorldRotation(CameraRotate);
		LevelupParticleComp->Activate(true);
	}
}
int32 AAuraCharacter::FindLevelFromAmountOfExp_Implementation(int32 Exp) const
{
	AAuraPlayerState* AuraPlayerState = GetPlayerState<AAuraPlayerState>();
	check(AuraPlayerState);

	return AuraPlayerState->LevelupInfo->FindLevelFromAmountOfExp(Exp);
}

int32 AAuraCharacter::GetAttributePointsReward_Implementation(int32 Level) const
{
	AAuraPlayerState* AuraPlayerState = GetPlayerState<AAuraPlayerState>();
	check(AuraPlayerState);

	return AuraPlayerState->LevelupInfo->LevelupInfo[Level].AttributePointReward;
}

int32 AAuraCharacter::GetSpellPointsReward_Implementation(int32 Level) const
{
	AAuraPlayerState* AuraPlayerState = GetPlayerState<AAuraPlayerState>();
	check(AuraPlayerState);

	return AuraPlayerState->LevelupInfo->LevelupInfo[Level].SpellPointReward;
}

int32 AAuraCharacter::GetAttributePoints_Implementation() const
{
	AAuraPlayerState* AuraPlayerState = GetPlayerState<AAuraPlayerState>();
	check(AuraPlayerState);

	return AuraPlayerState->GetAttributePoints();
}

int32 AAuraCharacter::GetSpellPoints_Implementation() const
{
	AAuraPlayerState* AuraPlayerState = GetPlayerState<AAuraPlayerState>();
	check(AuraPlayerState);

	return AuraPlayerState->GetSpellPoints();
}

void AAuraCharacter::AddToPlayerLevel_Implementation(int32 InPlayerLevel)
{
	AAuraPlayerState* AuraPlayerState = GetPlayerState<AAuraPlayerState>();
	check(AuraPlayerState);

	AuraPlayerState->AddLevel(InPlayerLevel);
	if (UAuraAbilitySystemComponent* AuraASC = Cast<UAuraAbilitySystemComponent>(GetAbilitySystemComponent())) {
		AuraASC->UpdateStatus(AuraPlayerState->GetPlayerLevel());
	};
}

void AAuraCharacter::AddToAttributePoints_Implementation(int32 InAttributePoints)
{
	AAuraPlayerState* AuraPlayerState = GetPlayerState<AAuraPlayerState>();
	check(AuraPlayerState);

	AuraPlayerState->AddAttributePoints(InAttributePoints);
}

void AAuraCharacter::AddToSpellPoints_Implementation(int32 InSpellPoints)
{
	AAuraPlayerState* AuraPlayerState = GetPlayerState<AAuraPlayerState>();
	check(AuraPlayerState);

	AuraPlayerState->AddSpellPoints(InSpellPoints);
}

int32 AAuraCharacter::GetPlayerLevel_Implementation()
{
	AAuraPlayerState* AuraPlayerState = GetPlayerState<AAuraPlayerState>();
	check(AuraPlayerState);

	return AuraPlayerState->GetPlayerLevel();
}

void AAuraCharacter::InitActorInfo()
{
	AAuraPlayerState* AuraPlayerState = GetPlayerState<AAuraPlayerState>();
	if (!AuraPlayerState) { return; };
	AuraPlayerState->GetAbilitySystemComponent()->InitAbilityActorInfo(AuraPlayerState, this);
	Cast<UAuraAbilitySystemComponent>(AuraPlayerState->GetAbilitySystemComponent())->AbilitySetInfo();

	AbilitySystemComp = AuraPlayerState->GetAbilitySystemComponent();
	AttributeSet = AuraPlayerState->GetAttributeSet();

	if (AAuraPlayerController* PlayerController = Cast<AAuraPlayerController>(GetController())) {
		if (AAuraHUD* AuraHUD = Cast<AAuraHUD>(PlayerController->GetHUD())) {
			AuraHUD->InitOverlay(AuraPlayerState,PlayerController,AbilitySystemComp,AttributeSet);
		}
	}

	InitDefaultAttributes();
}


