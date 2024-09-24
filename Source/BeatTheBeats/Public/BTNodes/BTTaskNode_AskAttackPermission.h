// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTaskNode_AskAttackPermission.generated.h"

/**
 * 
 */
UCLASS()
class BEATTHEBEATS_API UBTTaskNode_AskAttackPermission : public UBTTaskNode
{
	GENERATED_BODY()
	
public:

	UBTTaskNode_AskAttackPermission();

protected:

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
};
