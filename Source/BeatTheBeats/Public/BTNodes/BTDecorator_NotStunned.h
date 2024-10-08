// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTDecorator.h"
#include "BTDecorator_NotStunned.generated.h"

/**
 * 
 */
UCLASS()
class BEATTHEBEATS_API UBTDecorator_NotStunned : public UBTDecorator
{
	GENERATED_BODY()
	
public:

	UBTDecorator_NotStunned();

protected:

	virtual bool CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const override;
};
