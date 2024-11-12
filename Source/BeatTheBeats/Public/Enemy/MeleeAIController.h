// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "CrowdHandlingAIController.h"
#include "MeleeAIController.generated.h"

/**
 * 
 */
UCLASS()
class BEATTHEBEATS_API AMeleeAIController : public ACrowdHandlingAIController
{
	GENERATED_BODY()

public:

	AMeleeAIController(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:

	UPROPERTY(VisibleAnywhere)
	float RandomDistance = 0;

	UPROPERTY(VisibleAnywhere)
	bool WalkingRight = true;

	UPROPERTY(VisibleAnywhere)
	FVector TargetPosition;

	FVector LastPosition;

	UPROPERTY(VisibleAnywhere)
	float CurrentAngle;

private:

	UPROPERTY(EditDefaultsOnly)
	class UBehaviorTree* AIBehavior;

	UPROPERTY(EditDefaultsOnly)
	FName StartLocationKeyName = "StartLocation";
};
