// Fill out your copyright notice in the Description page of Project Settings.


#include "BTNodes/BTTaskNode_CheckMoveDist.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Enemy/MeleeAIController.h"
#include "Enemy/MeleeEnemy.h"

UBTTaskNode_CheckMoveDist::UBTTaskNode_CheckMoveDist()
{
	NodeName = TEXT("Check Move Distance");
}

EBTNodeResult::Type UBTTaskNode_CheckMoveDist::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);

	bool Valid = OwnerComp.GetBlackboardComponent()->GetValueAsBool(GetSelectedBlackboardKey());

	if (Valid) {
		return EBTNodeResult::Type::Succeeded;
	}
	
	return EBTNodeResult::Type::Failed;
}
