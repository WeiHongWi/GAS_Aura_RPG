// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "NiagaraComponent.h"
#include "GameplayTagContainer.h"
#include "DebuffNiagaraComp.generated.h"

struct FGameplayTag;
/**
 * 
 */
UCLASS()
class AURA_API UDebuffNiagaraComp : public UNiagaraComponent
{
	GENERATED_BODY()

public:
	UDebuffNiagaraComp();

	UPROPERTY(EditDefaultsOnly)
	FGameplayTag DebuffTag;
	
protected:
	virtual void BeginPlay() override;
	void DebuffChanged(const FGameplayTag DebuffTag, int32 NewCount);

	UFUNCTION()
	void DebuffDeactivate(AActor* DeathActor);
};
