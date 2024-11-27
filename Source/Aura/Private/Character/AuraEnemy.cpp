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

AAuraEnemy::AAuraEnemy()
{
	GetMesh()->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);

	AbilitySystemComp = CreateDefaultSubobject<UAuraAbilitySystemComponent>("AbilitySystemComp");
	AbilitySystemComp->SetIsReplicated(true);
	AbilitySystemComp->SetReplicationMode(EGameplayEffectReplicationMode::Minimal);

	AttributeSet = CreateDefaultSubobject<UAuraAttributeSet>("AttributeSet");
	HealthBar = CreateDefaultSubobject<UWidgetComponent>("HealthBar");
	HealthBar->SetupAttachment(GetRootComponent());
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

int32 AAuraEnemy::GetPlayerLevel()
{
	return level;
}


void AAuraEnemy::BeginPlay()
{
	Super::BeginPlay();
	InitActorInfo();
	UAuraAbilitySystemLibrary::InitializeDefualtGameplayAbility(this, AbilitySystemComp);

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
}


void AAuraEnemy::InitActorInfo()
{
	AbilitySystemComp->InitAbilityActorInfo(this, this);
	Cast<UAuraAbilitySystemComponent>(AbilitySystemComp)->AbilitySetInfo();

	InitDefaultAttributes();
}

void AAuraEnemy::InitDefaultAttributes()
{
	UAuraAbilitySystemLibrary::InitializeDefaultAttribute(this, CharacterClass, level, AbilitySystemComp);
}

void AAuraEnemy::Die()
{
	SetLifeSpan(LifeSpan);
	Super::Die();
}
