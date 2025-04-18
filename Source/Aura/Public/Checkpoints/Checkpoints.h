// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerStart.h"
#include "Interaction/SaveInterface.h"
#include "Checkpoints.generated.h"


class USphereComponent;

/**
 * 
 */
UCLASS()
class AURA_API ACheckpoints : public APlayerStart,public ISaveInterface
{
	GENERATED_BODY()
public:
	/*Save Interface*/
	virtual bool ShouldLoadTransform_Implementation() override { return false; };
	virtual void LoadActor_Implementation() override;

	ACheckpoints(const FObjectInitializer& ObjectInitializer);

	UPROPERTY(BlueprintReadOnly,SaveGame)
	bool bReached = false;
protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	virtual void OnSphereOverlap(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult
	);

	UFUNCTION(BlueprintImplementableEvent)
	void CheckpointReached(UMaterialInstanceDynamic* DynamicMaterialInstance); \

	void HandleGlowEffects();
private:
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UStaticMeshComponent> CheckpointMesh;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr <USphereComponent> SphereComp;
};
