// Fill out your copyright notice in the Description page of Project Settings.


#include "BTNodes/BTTaskNode_CommitToShoot.h"

UBTTaskNode_CommitToShoot::UBTTaskNode_CommitToShoot()
{
	NodeName = TEXT("Commit To Shooting");
}

EBTNodeResult::Type UBTTaskNode_CommitToShoot::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);
	//To Do: Add some shooting logic if needed.
	return EBTNodeResult::Type::Succeeded;
}
