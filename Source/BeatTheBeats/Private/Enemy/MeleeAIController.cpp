// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/MeleeAIController.h"
#include "Kismet/GameplayStatics.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"

AMeleeAIController::AMeleeAIController(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
}

void AMeleeAIController::BeginPlay()
{
	Super::BeginPlay();

	if (AIBehavior) {
		RunBehaviorTree(AIBehavior);

		GetBlackboardComponent()->SetValueAsVector(StartLocationKeyName, GetPawn()->GetActorLocation());
	}
}
