// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/AuraCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "AbilitySystemComponent.h"
#include "Player/AuraPlayerState.h"
#include "Player/AuraPlayerController.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "AbilitySystem/Data/LevelupInfo.h"
#include "AbilitySystem/AuraAttributeSet.h"
#include "AbilitySystem/AuraAbilitySystemLibrary.h"
#include "NiagaraComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "UI/HUD/AuraHUD.h"
#include "AuraGameplayTags.h"
#include "AbilitySystem/Debuff/DebuffNiagaraComp.h"
#include "Game/AuraGameModeBase.h"
#include "Game/LoadScreenSaveGame.h"
#include "Kismet/GameplayStatics.h"
#include "AbilitySystem/Data/AbilityInfo.h"



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
	LoadProgress();
	if (AAuraGameModeBase* AuraGameMode = Cast<AAuraGameModeBase>(UGameplayStatics::GetGameMode(this)))
	{
		AuraGameMode->LoadWorldState(GetWorld());
	}
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

int32 AAuraCharacter::GetPlayerCoin_Implementation() const
{
	AAuraPlayerState* AuraPlayerState = GetPlayerState<AAuraPlayerState>();
	check(AuraPlayerState);

	return AuraPlayerState->GetCoin();
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

void AAuraCharacter::AddToPlayerCoin_Implementation(int32 InCoin)
{
	AAuraPlayerState* AuraPlayerState = GetPlayerState<AAuraPlayerState>();
	check(AuraPlayerState);

	AuraPlayerState->AddCoin(InCoin);
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

void AAuraCharacter::ShowMagicCircle_Implementation(UMaterialInterface* DecalMaterial)
{
	AAuraPlayerController* AuraPlayerController = Cast<AAuraPlayerController>(GetController());
	check(AuraPlayerController);

	AuraPlayerController->ShowMagicCircle(DecalMaterial);
	AuraPlayerController->bShowMouseCursor = false;
}

void AAuraCharacter::HideMagicCircle_Implementation()
{
	AAuraPlayerController* AuraPlayerController = Cast<AAuraPlayerController>(GetController());
	check(AuraPlayerController);

	AuraPlayerController->HideMagicCircle();
	AuraPlayerController->bShowMouseCursor = true;
}

void AAuraCharacter::SaveProgress_Implementation(const FName& PlayerTag)
{
	AAuraGameModeBase* AuraGameMode = Cast<AAuraGameModeBase>(UGameplayStatics::GetGameMode(this));

	if (AuraGameMode) {
		ULoadScreenSaveGame* LoadScreenSaveGame = AuraGameMode->RetrieveInGameSaveData();
		if (!LoadScreenSaveGame) return;

		LoadScreenSaveGame->PlayerStartTag = PlayerTag;

		AAuraPlayerState* AuraPS = Cast<AAuraPlayerState>(GetPlayerState());
		if (AuraPS) {
			LoadScreenSaveGame->level = AuraPS->GetPlayerLevel();
			LoadScreenSaveGame->exp = AuraPS->GetXP();
			LoadScreenSaveGame->AttributePoints = AuraPS->GetAttributePoints();
			LoadScreenSaveGame->SpellPoints = AuraPS->GetSpellPoints();
		}

		LoadScreenSaveGame->Strength = UAuraAttributeSet::GetStrengthAttribute().GetNumericValue(GetAttributeSet());
		LoadScreenSaveGame->Intelligence = UAuraAttributeSet::GetIntelligenceAttribute().GetNumericValue(GetAttributeSet());
		LoadScreenSaveGame->Resilience = UAuraAttributeSet::GetResilienceAttribute().GetNumericValue(GetAttributeSet());
		LoadScreenSaveGame->Vigor = UAuraAttributeSet::GetVigorAttribute().GetNumericValue(GetAttributeSet());
		
		LoadScreenSaveGame->bFirstLoadData = false;
		
		if (!HasAuthority()) return;

		UAuraAbilitySystemComponent* AuraASC = Cast<UAuraAbilitySystemComponent>(AbilitySystemComp);

		FForEachAbility ForEachAbilityDelegate;
		LoadScreenSaveGame->SaveGameAbilities.Empty();
		ForEachAbilityDelegate.BindLambda([this, AuraASC, LoadScreenSaveGame](const FGameplayAbilitySpec& spec) {
			FSaveGameAbility SaveGameAbility;
			FGameplayTag AbilityTag = UAuraAbilitySystemComponent::GetAbilityTagBySpec(spec);
			UAbilityInfo* AbilityInfo = UAuraAbilitySystemLibrary::GetAbilityInfo(this);
			FAuraAbilityInfo Info = AbilityInfo->GetAbilityInfoByTag(AbilityTag);

			SaveGameAbility.AbilityTag = AbilityTag;
			SaveGameAbility.level = spec.Level;
			SaveGameAbility.GameplayAbility = Info.Ability;
			SaveGameAbility.AbilityType = Info.AbilityType;
			SaveGameAbility.AbilityStatus = AuraASC->GetStatusByAbilityTag(AbilityTag);
			SaveGameAbility.AbilitySlot = AuraASC->GetSlotByAbilityTag(AbilityTag);

			LoadScreenSaveGame->SaveGameAbilities.AddUnique(SaveGameAbility);
		});
		AuraASC->ForEachAbility(ForEachAbilityDelegate);
		AuraGameMode->SaveInProgressData(LoadScreenSaveGame);
	}
}

int32 AAuraCharacter::GetPlayerLevel_Implementation()
{
	AAuraPlayerState* AuraPlayerState = GetPlayerState<AAuraPlayerState>();
	check(AuraPlayerState);

	return AuraPlayerState->GetPlayerLevel();
}


void AAuraCharacter::StunTagChanged(const FGameplayTag CallbackTag, int32 NewCount)
{
	Super::StunTagChanged(CallbackTag, NewCount);
}

void AAuraCharacter::OnRep_Stunned()
{
	if (UAuraAbilitySystemComponent* AuraASC = Cast<UAuraAbilitySystemComponent>(AbilitySystemComp)) {
		const FAuraGameplayTags GameplayTags = FAuraGameplayTags::Get();
		FGameplayTagContainer BlockedTags;

		BlockedTags.AddTag(GameplayTags.Player_Block_CursorTrace);
		BlockedTags.AddTag(GameplayTags.Player_Block_InputHeld);
		BlockedTags.AddTag(GameplayTags.Player_Block_InputPressed);
		BlockedTags.AddTag(GameplayTags.Player_Block_InputReleased);

		if (bIsStunned)
		{
			AuraASC->AddLooseGameplayTags(BlockedTags);
			StunNiagaraComp->Activate();
		}
		else
		{
			AuraASC->RemoveLooseGameplayTags(BlockedTags);
			StunNiagaraComp->Deactivate();
		}
	}
}

void AAuraCharacter::OnRep_Burned()
{
	if (bIsBurned)
	{
		BurnNiagaraComp->Activate();
	}
	else
	{
		BurnNiagaraComp->Deactivate();
	}
}

void AAuraCharacter::InitActorInfo()
{
	AAuraPlayerState* AuraPlayerState = GetPlayerState<AAuraPlayerState>();
	if (!AuraPlayerState) { return; };
	AuraPlayerState->GetAbilitySystemComponent()->InitAbilityActorInfo(AuraPlayerState, this);
	Cast<UAuraAbilitySystemComponent>(AuraPlayerState->GetAbilitySystemComponent())->AbilitySetInfo();

	AbilitySystemComp = AuraPlayerState->GetAbilitySystemComponent();
	OnAscRegistered.Broadcast(AbilitySystemComp);

	FDelegateHandle handle =  AbilitySystemComp->RegisterGameplayTagEvent(
		FAuraGameplayTags::Get().Debuff_Stun, 
		EGameplayTagEventType::NewOrRemoved
		).AddUObject(this,&AAuraCharacter::StunTagChanged);

	
	AttributeSet = AuraPlayerState->GetAttributeSet();

	if (AAuraPlayerController* PlayerController = Cast<AAuraPlayerController>(GetController())) {
		if (AAuraHUD* AuraHUD = Cast<AAuraHUD>(PlayerController->GetHUD())) {
			AuraHUD->InitOverlay(AuraPlayerState,PlayerController,AbilitySystemComp,AttributeSet);
		}
	}
}

void AAuraCharacter::LoadProgress()
{
	AAuraGameModeBase* AuraGameMode = Cast<AAuraGameModeBase>(UGameplayStatics::GetGameMode(this));
	if (AuraGameMode) {
		ULoadScreenSaveGame* LoadScreenSaveGame = AuraGameMode->RetrieveInGameSaveData();
		if (!LoadScreenSaveGame) return;

		if (LoadScreenSaveGame->bFirstLoadData) {
			InitDefaultAttributes();
			AddCharacterAbilities();
		}
		else {
			if (UAuraAbilitySystemComponent* AuraASC = Cast<UAuraAbilitySystemComponent>(AbilitySystemComp)) {
				AuraASC->AddCharacterAbilitiesFromSaveData(LoadScreenSaveGame);
			}

			if (AAuraPlayerState* AuraPS = Cast<AAuraPlayerState>(GetPlayerState())) {
				AuraPS->SetLevel(LoadScreenSaveGame->level);
				AuraPS->SetXP(LoadScreenSaveGame->exp);
				AuraPS->SetAttributePoints(LoadScreenSaveGame->AttributePoints);
				AuraPS->SetSpellPoints(LoadScreenSaveGame->SpellPoints);
			}
			UAuraAbilitySystemLibrary::InitializeDefaultAttribute_SaveGame(this, AbilitySystemComp, LoadScreenSaveGame);
		}
	}
}


