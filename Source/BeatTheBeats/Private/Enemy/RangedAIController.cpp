// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/RangedAIController.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"

ARangedAIController::ARangedAIController(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
}

void ARangedAIController::BeginPlay()
{
	Super::BeginPlay();

	if (AIBehavior) {
		RunBehaviorTree(AIBehavior);

		GetBlackboardComponent()->SetValueAsVector(StartLocationKeyName, GetPawn()->GetActorLocation());
	}
}
