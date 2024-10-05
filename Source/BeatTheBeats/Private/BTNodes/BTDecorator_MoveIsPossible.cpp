// Fill out your copyright notice in the Description page of Project Settings.


#include "BTNodes/BTDecorator_MoveIsPossible.h"
#include "Enemy/MeleeAIController.h"
#include "BTNodes/BTTask_GetPositionNearPlayer.h"

UBTDecorator_MoveIsPossible::UBTDecorator_MoveIsPossible() {
	NodeName = TEXT("Move Is Possible?");
}

bool UBTDecorator_MoveIsPossible::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const {
	
	AMeleeAIController* controller = Cast<AMeleeAIController>(OwnerComp.GetAIOwner());

	if (controller) {

		return UBTTask_GetPositionNearPlayer::GetValidPosition(controller);
	}

	return true;
}