// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Enemy/EnemyBase.h"
#include "RangedEnemy.generated.h"

/**
 * 
 */
UCLASS()
class BEATTHEBEATS_API ARangedEnemy : public AEnemyBase
{
	GENERATED_BODY()
	
public:
	// Sets default values for this character's properties
	ARangedEnemy();

	virtual bool GetCanAttack() override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void OnBeat(float CurrentTimeSinceLastBeat) override;

	virtual void Attack() override;

	virtual void DoDamage() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

private:

	UPROPERTY(VisibleAnywhere)
	USceneComponent* ShootPoint;

	UPROPERTY(EditDefaultsOnly)
	UParticleSystem* HitEffect;

	UPROPERTY(EditDefaultsOnly)
	UParticleSystem* MuzzleEffect;

	UPROPERTY(EditDefaultsOnly)
	float CloseQuarterDistance = 200;
};
