// Fill out your copyright notice in the Description page of Project Settings.


#include "BTNodes/BTTaskNode_CommitToAttack.h"

UBTTaskNode_CommitToAttack::UBTTaskNode_CommitToAttack()
{
	NodeName = TEXT("Commit To Attack");
}

EBTNodeResult::Type UBTTaskNode_CommitToAttack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);
	//To Do: Add logic to update Attack Visuals.
	return EBTNodeResult::Type::Succeeded;
}
