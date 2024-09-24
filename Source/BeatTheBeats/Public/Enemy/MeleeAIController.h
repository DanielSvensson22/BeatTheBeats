// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "MeleeAIController.generated.h"

/**
 * 
 */
UCLASS()
class BEATTHEBEATS_API AMeleeAIController : public AAIController
{
	GENERATED_BODY()
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:

	void SetFocusState(AActor* focus, bool ShouldFocus);

private:

	UPROPERTY(EditDefaultsOnly)
		class UBehaviorTree* AIBehavior;

	UPROPERTY(EditDefaultsOnly)
	FName StartLocationKeyName = "StartLocation";

	bool bHasSetFocus = false;
};