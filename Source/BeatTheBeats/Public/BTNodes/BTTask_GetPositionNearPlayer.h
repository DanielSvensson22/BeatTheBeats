// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "BTTask_GetPositionNearPlayer.generated.h"

class AMeleeAIController;

/**
 * 
 */
UCLASS()
class BEATTHEBEATS_API UBTTask_GetPositionNearPlayer : public UBTTask_BlackboardBase
{
	GENERATED_BODY()
	
public:

	UBTTask_GetPositionNearPlayer();

protected:

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

private:

	void GetValidPosition(AMeleeAIController* controller);
};
