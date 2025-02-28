// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/AuraEnemy.h"

#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "AbilitySystem/AuraAttributeSet.h"
#include "Aura/Aura.h"
#include "Components/WidgetComponent.h"
#include "UI/Widget/AuraUserWidget.h"
#include "AbilitySystem/AuraAbilitySystemLibrary.h"
#include "AuraGameplayTags.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "AI/AuraAIController.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"

AAuraEnemy::AAuraEnemy()
{
	GetMesh()->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);

	AbilitySystemComp = CreateDefaultSubobject<UAuraAbilitySystemComponent>("AbilitySystemComp");
	AbilitySystemComp->SetIsReplicated(true);
	AbilitySystemComp->SetReplicationMode(EGameplayEffectReplicationMode::Minimal);

	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;
	bUseControllerRotationYaw = false;
	GetCharacterMovement()->bUseControllerDesiredRotation = true;
	
	
	AttributeSet = CreateDefaultSubobject<UAuraAttributeSet>("AttributeSet");
	HealthBar = CreateDefaultSubobject<UWidgetComponent>("HealthBar");
	HealthBar->SetupAttachment(GetRootComponent());
}

void AAuraEnemy::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	if (!HasAuthority()) {
		return;
	}

	EnemyAIController = Cast<AAuraAIController>(NewController);
	EnemyAIController->GetBlackboardComponent()->InitializeBlackboard(*BehaviorTree->BlackboardAsset);
	EnemyAIController->RunBehaviorTree(BehaviorTree);
	EnemyAIController->GetBlackboardComponent()->SetValueAsBool(FName("bIsHitReact"), false);
	EnemyAIController->GetBlackboardComponent()->SetValueAsBool(FName("RangedAttacker"), CharacterClass != ECharacterClass::Warrior);
}

void AAuraEnemy::HighLight()
{
	GetMesh()->SetRenderCustomDepth(true);
	GetMesh()->SetCustomDepthStencilValue(CUSTOM_DEPTH_RED);

	Weapon->SetRenderCustomDepth(true);
	Weapon->SetCustomDepthStencilValue(CUSTOM_DEPTH_RED);
}

void AAuraEnemy::UnHighLight()
{
	GetMesh()->SetRenderCustomDepth(false);
	Weapon->SetRenderCustomDepth(false);
}

AActor* AAuraEnemy::GetCombatTarget_Implementation()
{
	return CombatTarget;
}

void AAuraEnemy::SetCombatTarget_Implementation(AActor* Target)
{
	CombatTarget = Target;
}

int32 AAuraEnemy::GetPlayerLevel_Implementation()
{
	return level;
}


void AAuraEnemy::BeginPlay()
{
	Super::BeginPlay();
	InitActorInfo();
	if (HasAuthority()) {
		UAuraAbilitySystemLibrary::InitializeDefualtGameplayAbility(this, AbilitySystemComp,CharacterClass);
	}
	//Controller Set
	if (UAuraUserWidget* AuraWidget = Cast<UAuraUserWidget>(HealthBar->GetUserWidgetObject())) {
		AuraWidget->SetWidgerController(this);
	}
	if (const UAuraAttributeSet* AuraAS = CastChecked<UAuraAttributeSet>(AttributeSet))
	{
		AbilitySystemComp->GetGameplayAttributeValueChangeDelegate(AuraAS->GetHealthAttribute()).AddLambda(
			[this](const FOnAttributeChangeData& Data)
			{
				OnHealthChange.Broadcast(Data.NewValue);
			}
		);
		AbilitySystemComp->GetGameplayAttributeValueChangeDelegate(AuraAS->GetMaxHealthAttribute()).AddLambda(
			[this](const FOnAttributeChangeData& Data)
			{
				OnMaxHealthChange.Broadcast(Data.NewValue);
			}
		);
		AbilitySystemComp->RegisterGameplayTagEvent(
			FAuraGameplayTags::Get().HitReact, 
			EGameplayTagEventType::NewOrRemoved
		).AddUObject(this,&AAuraEnemy::HitReactTagChanged);



		OnHealthChange.Broadcast(AuraAS->GetHealth());
		OnMaxHealthChange.Broadcast(AuraAS->GetMaxHealth());
	}
}

void AAuraEnemy::HitReactTagChanged(const FGameplayTag GameplayTag, int32 NewCount)
{
	bHitReact = NewCount > 0;
	GetCharacterMovement()->MaxWalkSpeed = (bHitReact) ? 0.f : BaseWalkSpeed;
	if (EnemyAIController && EnemyAIController->GetBlackboardComponent()) {
		EnemyAIController->GetBlackboardComponent()->SetValueAsBool(FName("bIsHitReact"), bHitReact);
	}
}


void AAuraEnemy::InitActorInfo()
{
	AbilitySystemComp->InitAbilityActorInfo(this, this);
	Cast<UAuraAbilitySystemComponent>(AbilitySystemComp)->AbilitySetInfo();
	
	if (HasAuthority()) {
		InitDefaultAttributes();
	}

	OnAscRegistered.Broadcast(AbilitySystemComp);
}

void AAuraEnemy::InitDefaultAttributes()
{
	UAuraAbilitySystemLibrary::InitializeDefaultAttribute(this, CharacterClass, level, AbilitySystemComp);
}

void AAuraEnemy::Die()
{
	SetLifeSpan(LifeSpan);
	if (EnemyAIController) {
		EnemyAIController->GetBlackboardComponent()->SetValueAsBool(FName("Dead"), true);
	}
	Super::Die();
}
