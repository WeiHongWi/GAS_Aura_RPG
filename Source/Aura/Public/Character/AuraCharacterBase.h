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
struct FGameplayTag;
class UPassiveNiagaraComponent;

UCLASS()
class AURA_API AAuraCharacterBase : public ACharacter, public IAbilitySystemInterface, public ICombatInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AAuraCharacterBase();

	FOnASCRegistered OnAscRegistered;
	FOnDeath OnDeath;
	FOnDamage OnDamage;

	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	virtual float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;
	virtual void Tick(float DeltaTime) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const;
	UAttributeSet* GetAttributeSet() const { return AttributeSet; }

	UFUNCTION(NetMulticast, Reliable)
	void MulticastHandleDeath(const FVector Impulse);

	//Combat Interface Begin
	virtual UAnimMontage* GetHitReactAnimMontage_Implementation() override;
	virtual void Die(const FVector Impulse) override;
	
	virtual FVector GetWeaponTipSocketLocation_Implementation(const FGameplayTag& MontageTag) override;
	virtual bool bIsDead_Implementation() const override;
	virtual AActor* GetAvatarActor_Implementation() override;
	virtual TArray<FTagMontage> GetAttackMontages_Implementation() override;
	virtual UNiagaraSystem* GetBloodEffect_Implementation() override;
	virtual FTagMontage GetTagMontageByTag_Implementation(const FGameplayTag& MontageTag) override;
	virtual int GetMinionCount_Implementation() override;
	virtual void IncreaseMinionCount_Implementation(int NumOfMinions) override;
	virtual FOnASCRegistered& GetOnASCRegisterDelegate() override;
	virtual FOnDeath& GetOnDeathDelegate() override;
	virtual FOnDamage& GetOnDamageDelegate() override;
	virtual USkeletalMeshComponent* GetWeapon_Implementation() override;
	virtual void SetIsBeingShocked_Implementation(bool bInShock) override;
	virtual bool IsBeingShocked_Implementation() const override;

	//Combat Interface End

	bool IsDead = false;

	UPROPERTY(EditAnywhere, Category = "Combat")
	TArray<FTagMontage> AttackMontages;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UDebuffNiagaraComp> BurnNiagaraComp;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UDebuffNiagaraComp> StunNiagaraComp;
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

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat")
	float BaseWalkSpeed = 600.f;

	UPROPERTY(ReplicatedUsing=OnRep_Stunned, BlueprintReadOnly)
	bool bIsStunned;

	UPROPERTY(ReplicatedUsing = OnRep_Burned, BlueprintReadOnly)
	bool bIsBurned;

	UPROPERTY(Replicated, BlueprintReadOnly)
	bool bIsBeingShockLoop = false;

	UFUNCTION()
	virtual void OnRep_Burned();

	UFUNCTION()
	virtual void OnRep_Stunned();

	virtual void StunTagChanged(const FGameplayTag CallbackTag, int32 NewCount);
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

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UPassiveNiagaraComponent> HaloProtectionNiagaraComponent;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UPassiveNiagaraComponent> LifeSiphonNiagaraComponent;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UPassiveNiagaraComponent> ManaSiphonNiagaraComponent;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USceneComponent> EffectAttachComponent;
};
