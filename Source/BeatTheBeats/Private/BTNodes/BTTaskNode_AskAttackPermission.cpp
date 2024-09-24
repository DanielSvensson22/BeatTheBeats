// Fill out your copyright notice in the Description page of Project Settings.


#include "BTNodes/BTTaskNode_AskAttackPermission.h"
#include "AIController.h"
#include "Enemy/EnemyBase.h"

UBTTaskNode_AskAttackPermission::UBTTaskNode_AskAttackPermission() {
	NodeName = TEXT("Ask For Attack Permission");
}

EBTNodeResult::Type UBTTaskNode_AskAttackPermission::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) {
	AEnemyBase* enemy = Cast<AEnemyBase>(OwnerComp.GetAIOwner()->GetPawn());

	if (enemy) {
		enemy->EnterQueue();
		return EBTNodeResult::Type::Succeeded;
	}

	return EBTNodeResult::Type::Failed;
}