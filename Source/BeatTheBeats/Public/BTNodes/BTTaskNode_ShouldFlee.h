// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "BTTaskNode_ShouldFlee.generated.h"

/**
 * 
 */
UCLASS()
class BEATTHEBEATS_API UBTTaskNode_ShouldFlee : public UBTTask_BlackboardBase
{
	GENERATED_BODY()
	
public:

	UBTTaskNode_ShouldFlee();

protected:

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
};
