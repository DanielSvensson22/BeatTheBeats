// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTDecorator.h"
#include "BTDecorator_MoveIsPossible.generated.h"

/**
 * 
 */
UCLASS()
class BEATTHEBEATS_API UBTDecorator_MoveIsPossible : public UBTDecorator
{
	GENERATED_BODY()
	
public:

	UBTDecorator_MoveIsPossible();

protected:

	virtual bool CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const override;
};
