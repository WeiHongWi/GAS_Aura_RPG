// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AbilitySystemInterface.h"
#include "Interaction/CombatInterface.h"
#include "AuraCharacterBase.generated.h"

class UAbilitySystemComponent;
class UAttributeSet;
class UGameplayEffect;
class UGameplayAbility;
class UAnimMontage;

UCLASS()
class AURA_API AAuraCharacterBase : public ACharacter, public IAbilitySystemInterface, public ICombatInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AAuraCharacterBase();

	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	UAttributeSet* GetAttributeSet() const { return AttributeSet; }

	UFUNCTION(NetMulticast, Reliable)
	void MulticastHandleDeath();

	virtual UAnimMontage* GetHitReactAnimMontage_Implementation() override;

	virtual void Die() override;
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	UPROPERTY(EditAnywhere,Category="Combat")
	TObjectPtr<USkeletalMeshComponent> Weapon;

	UPROPERTY(EditAnywhere, Category = "Combat")
	FName WeaponTip;

	UPROPERTY()
	TObjectPtr<UAbilitySystemComponent> AbilitySystemComp;

	UPROPERTY()
	TObjectPtr<UAttributeSet> AttributeSet;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Attributes")
	TSubclassOf<UGameplayEffect> DefaultPrimaryAttribute;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Attributes")
	TSubclassOf<UGameplayEffect> DefaultSecondaryAttribute;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Attributes")
	TSubclassOf<UGameplayEffect> DefaultVitalAttribute;

	virtual void InitActorInfo();

	virtual FVector GetPartSocketLocation() override;

	void ApplyDefaultAttribute(TSubclassOf<UGameplayEffect> Attribute, float level);

	virtual void InitDefaultAttributes();

	void AddCharacterAbilities();

	void Disslove();

	UFUNCTION(BlueprintImplementableEvent)
	void StartDissloveEvent(UMaterialInstanceDynamic* MI);

	UFUNCTION(BlueprintImplementableEvent)
	void StartWeaponDissloveEvent(UMaterialInstanceDynamic* MI);
private:
	UPROPERTY(EditAnywhere, Category = "Abilities")
	TArray < TSubclassOf < UGameplayAbility >> StartUpAbilities;

	UPROPERTY(EditAnywhere, Category = "Combat")
	TObjectPtr<UAnimMontage> AnimMontage;

	UPROPERTY(EditAnywhere,Category = "Material")
	TObjectPtr<UMaterialInstance> DynamicMaterialDisslove;

	UPROPERTY(EditAnywhere, Category = "Material")
	TObjectPtr<UMaterialInstance> DynamicMaterialWeaponDisslove;

};
