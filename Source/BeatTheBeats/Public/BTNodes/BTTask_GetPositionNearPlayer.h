// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "BTTask_GetPositionNearPlayer.generated.h"

class AMeleeAIController;
class AEnemyBase;

/**
 * 
 */
UCLASS()
class BEATTHEBEATS_API UBTTask_GetPositionNearPlayer : public UBTTask_BlackboardBase
{
	GENERATED_BODY()
	
public:

	UBTTask_GetPositionNearPlayer();

	static bool GetValidPosition(AMeleeAIController* controller);

protected:

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

private:

	virtual void InitializeFromAsset(UBehaviorTree& Asset) override;

	bool GetValidPath(AMeleeAIController* controller, AEnemyBase* enemy);

	UPROPERTY(EditAnywhere, Category = Blackboard)
		struct FBlackboardKeySelector BlackboardValidMoveKey;
};
