// Fill out your copyright notice in the Description page of Project Settings.


#include "BTNodes/BTDecorator_NotStunned.h"
#include "AIController.h"
#include "Enemy/EnemyBase.h"

UBTDecorator_NotStunned::UBTDecorator_NotStunned()
{
	NodeName = TEXT("Not Stunned?");
}

bool UBTDecorator_NotStunned::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
	Super::CalculateRawConditionValue(OwnerComp, NodeMemory);

	AEnemyBase* enemy = Cast<AEnemyBase>(OwnerComp.GetAIOwner()->GetPawn());

	if (enemy) {
		return !(enemy->GetIsStunned());
	}

	return false;
}
