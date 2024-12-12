// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Enemy/MeleeEnemy.h"
#include "TricksterMelee.generated.h"

/**
 * 
 */
UCLASS()
class BEATTHEBEATS_API ATricksterMelee : public AMeleeEnemy
{
	GENERATED_BODY()
	
public:

	virtual float ApplyDamage(float Damage, Attacks AttackType, bool OnBeat, FVector HitLocation) override;

protected:

	virtual void ApplyDamageEffects(float FinalDamage, bool OnBeat) override;

private:

	bool bHitByOwnType = false;
};
