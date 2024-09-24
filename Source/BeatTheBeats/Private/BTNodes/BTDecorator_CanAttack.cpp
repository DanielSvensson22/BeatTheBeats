// Fill out your copyright notice in the Description page of Project Settings.


#include "BTNodes/BTDecorator_CanAttack.h"
#include "AIController.h"
#include "Enemy/EnemyBase.h"

UBTDecorator_CanAttack::UBTDecorator_CanAttack()
{
	NodeName = TEXT("Can Attack?");
}

bool UBTDecorator_CanAttack::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
	Super::CalculateRawConditionValue(OwnerComp, NodeMemory);

	AEnemyBase* enemy = Cast<AEnemyBase>(OwnerComp.GetAIOwner()->GetPawn());

	if (enemy) {
		return enemy->GetCanAttack();
	}

	return false;
}
