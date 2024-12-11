// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "../Combos/Combo.h"
#include "HitInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UHitInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class BEATTHEBEATS_API IHitInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	virtual void GetHit(const FVector& ImpactPoint) = 0;

	virtual float ApplyDamage(float Damage, Attacks AttackType, bool OnBeat, FVector HitLocation) = 0;

	virtual void ApplyPushBack(float Force) = 0;
};
