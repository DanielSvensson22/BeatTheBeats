// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/RangedAIController.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"

void ARangedAIController::BeginPlay()
{
	Super::BeginPlay();

	if (AIBehavior) {
		RunBehaviorTree(AIBehavior);

		GetBlackboardComponent()->SetValueAsVector(StartLocationKeyName, GetPawn()->GetActorLocation());
	}
}
