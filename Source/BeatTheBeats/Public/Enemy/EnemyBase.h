// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "../Combos/Combo.h"
#include "EnemyBase.generated.h"

class ABeatManager;
class AEnemyQueue;

UCLASS()
class BEATTHEBEATS_API AEnemyBase : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AEnemyBase();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void OnBeat(float CurrentTimeSinceLastBeat);

	virtual void Attack();

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual bool GetCanAttack();

	FORCEINLINE float GetMaxAttackDistance() { return MaxAttackDistance; }
	FORCEINLINE bool GetCanChasePlayer() { return bCanChasePlayer; }
	FORCEINLINE bool IsAlive() { return CurrentHealth > 0; }

	FORCEINLINE Attacks GetEnemyType() { return EnemyType; }

	void SetAttackState(bool CanAttack, bool CanChase);

	void EnterQueue();

	virtual void Parry();

protected:

	class ABeatManager* BeatManager;

	UPROPERTY(EditDefaultsOnly)
	FCombo StandardCombo;

	UPROPERTY(EditDefaultsOnly)
	Attacks EnemyType;

	int CurrentAttack;

	bool bCanAttack = false;

	UPROPERTY(EditAnywhere)
	float MaxHealth = 100;

	UPROPERTY(VisibleAnywhere)
	float CurrentHealth;

	UPROPERTY(EditDefaultsOnly)
	float AttackRange = 100;

	UPROPERTY(EditDefaultsOnly)
	float Damage = 10;

	UPROPERTY(VisibleAnywhere)
	bool bIsMelee = true;

protected:

	virtual void DoDamage();

private:

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<ABeatManager> BeatManagerClass;

	APawn* Player;

	UPROPERTY(EditDefaultsOnly)
	float MaxAttackDistance = 100;

	bool bCanChasePlayer = false;

	bool bHasAddedToQueue = false;

	AEnemyQueue* EnemyQueue;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<AEnemyQueue> EnemyQueueClass;
};
