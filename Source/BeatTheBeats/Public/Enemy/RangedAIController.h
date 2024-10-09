// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "RangedAIController.generated.h"

/**
 * 
 */
UCLASS()
class BEATTHEBEATS_API ARangedAIController : public AAIController
{
	GENERATED_BODY()
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:

	UPROPERTY(EditDefaultsOnly)
	class UBehaviorTree* AIBehavior;

	UPROPERTY(EditDefaultsOnly)
	FName StartLocationKeyName = "StartLocation";
};
