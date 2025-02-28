// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AbilitySystemInterface.h"
#include "Interaction/CombatInterface.h"
#include "AbilitySystem/Data/CharacterClassInfo.h"
#include "AuraCharacterBase.generated.h"

class UAbilitySystemComponent;
class UAttributeSet;
class UGameplayEffect;
class UGameplayAbility;
class UAnimMontage;
class UNiagaraSystem;
class UDebuffNiagaraComp;

UCLASS()
class AURA_API AAuraCharacterBase : public ACharacter, public IAbilitySystemInterface, public ICombatInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AAuraCharacterBase();

	FOnASCRegistered OnAscRegistered;
	FOnDeath OnDeath;

	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	UAttributeSet* GetAttributeSet() const { return AttributeSet; }

	UFUNCTION(NetMulticast, Reliable)
	void MulticastHandleDeath();

	//Combat Interface Begin

	virtual UAnimMontage* GetHitReactAnimMontage_Implementation() override;
	virtual void Die() override;
	virtual FVector GetWeaponTipSocketLocation_Implementation(const FGameplayTag& MontageTag) override;
	virtual bool bIsDead_Implementation() const override;
	virtual AActor* GetAvatarActor_Implementation() override;
	virtual TArray<FTagMontage> GetAttackMontages_Implementation() override;
	virtual UNiagaraSystem* GetBloodEffect_Implementation() override;
	virtual FTagMontage GetTagMontageByTag_Implementation(const FGameplayTag& MontageTag) override;
	virtual int GetMinionCount_Implementation() override;
	virtual void IncreaseMinionCount_Implementation(int NumOfMinions) override;
	virtual FOnASCRegistered GetOnASCRegisterDelegate() override;
	virtual FOnDeath GetOnDeathDelegate() override;
	//Combat Interface End

	bool IsDead = false;

	UPROPERTY(EditAnywhere, Category = "Combat")
	TArray<FTagMontage> AttackMontages;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UDebuffNiagaraComp> BurnNiagaraComp;
protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Enemy Character Class Default")
	ECharacterClass CharacterClass;

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	
	
	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category="Combat")
	TObjectPtr<USkeletalMeshComponent> Weapon;

	UPROPERTY(EditAnywhere, Category = "Combat")
	FName WeaponTip;

	UPROPERTY(EditAnywhere, Category = "Combat")
	FName LeftHand;

	UPROPERTY(EditAnywhere, Category = "Combat")
	FName RightHand;

	UPROPERTY(EditAnywhere, Category = "Combat")
	FName TailSocket;

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

	virtual ECharacterClass GetCharacterClass_Implementation() override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat")
	UNiagaraSystem* BloodEffect;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat")
	USoundBase* DeathSound;

	int MinionCount = 0;
private:
	UPROPERTY(EditAnywhere, Category = "Abilities")
	TArray < TSubclassOf < UGameplayAbility >> StartUpAbilities;

	UPROPERTY(EditAnywhere, Category = "Abilities")
	TArray < TSubclassOf < UGameplayAbility >> StartUpPassiveAbilities;

	UPROPERTY(EditAnywhere, Category = "Combat")
	TObjectPtr<UAnimMontage> AnimMontage;

	UPROPERTY(EditAnywhere,Category = "Material")
	TObjectPtr<UMaterialInstance> DynamicMaterialDisslove;

	UPROPERTY(EditAnywhere, Category = "Material")
	TObjectPtr<UMaterialInstance> DynamicMaterialWeaponDisslove;

};
