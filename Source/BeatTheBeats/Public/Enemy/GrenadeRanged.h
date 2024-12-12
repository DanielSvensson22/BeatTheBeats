// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Enemy/RangedEnemy.h"
#include "GrenadeRanged.generated.h"

class AEnemyProjectile;

/**
 * 
 */
UCLASS()
class BEATTHEBEATS_API AGrenadeRanged : public ARangedEnemy
{
	GENERATED_BODY()
	
protected:

	virtual void Attack() override;

	virtual void DoDamage() override;

private:

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<AEnemyProjectile> ProjectileClass;
};
