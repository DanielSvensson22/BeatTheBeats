// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "CrowdHandlingAIController.generated.h"

/**
 * 
 */
UCLASS()
class BEATTHEBEATS_API ACrowdHandlingAIController : public AAIController
{
	GENERATED_BODY()
	
public:
	ACrowdHandlingAIController(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	void SetFocusState(AActor* focus, bool ShouldFocus);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:
	bool bHasSetFocus = false;
};
