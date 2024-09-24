// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include <queue>
#include "EnemyQueue.generated.h"

class AEnemyBase;

UCLASS()
class BEATTHEBEATS_API AEnemyQueue : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AEnemyQueue();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void AddToQueue(AEnemyBase* Enemy);

	void RemoveEnemy(AEnemyBase* Enemy);

private:

	std::queue<AEnemyBase*> WaitingEnemies;

	UPROPERTY(EditDefaultsOnly)
	int MaxAttackingEnemies = 2;

	int CurrentEnemyCount = 0;

	TArray<AEnemyBase*> AttackingEnemies;
};