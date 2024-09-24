// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTaskNode_CommitToAttack.generated.h"

/**
 * 
 */
UCLASS()
class BEATTHEBEATS_API UBTTaskNode_CommitToAttack : public UBTTaskNode
{
	GENERATED_BODY()
	
public:

	UBTTaskNode_CommitToAttack();

protected:

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
};
