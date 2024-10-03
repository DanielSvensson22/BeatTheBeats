// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Enemy/EnemyBase.h"
#include "MeleeEnemy.generated.h"

/**
 * 
 */
UCLASS()
class BEATTHEBEATS_API AMeleeEnemy : public AEnemyBase
{
	GENERATED_BODY()
	
public:
	// Sets default values for this character's properties
	AMeleeEnemy();

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

	FORCEINLINE float GetMinStalkDistance() { return MinStalkDistance; }
	FORCEINLINE float GetMaxStalkDistance() { return MaxStalkDistance; }
	FORCEINLINE float GetStalkSpeed() { return StalkSpeed; }

private:

	UPROPERTY(VisibleAnywhere)
	USceneComponent* AttackPoint;

	UPROPERTY(EditDefaultsOnly)
	UParticleSystem* HitEffect;

	float LastDistanceToPlayer;

	UPROPERTY(EditDefaultsOnly)
	float MinStalkDistance = 300;

	UPROPERTY(EditDefaultsOnly)
	float MaxStalkDistance = 1500;

	UPROPERTY(EditDefaultsOnly)
	float StalkSpeed = 10;
};
