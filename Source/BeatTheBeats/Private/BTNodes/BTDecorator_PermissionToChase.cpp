// Fill out your copyright notice in the Description page of Project Settings.


#include "BTNodes/BTDecorator_PermissionToChase.h"
#include "AIController.h"
#include "Enemy/EnemyBase.h"

UBTDecorator_PermissionToChase::UBTDecorator_PermissionToChase() {
	NodeName = TEXT("Permission To Chase");
}

bool UBTDecorator_PermissionToChase::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
	AEnemyBase* enemy = Cast<AEnemyBase>(OwnerComp.GetAIOwner()->GetPawn());

	if (enemy) {
		return enemy->GetCanChasePlayer();
	}

	return false;
}