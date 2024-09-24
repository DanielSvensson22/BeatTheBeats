// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/EnemyQueue.h"
#include "Enemy/EnemyBase.h"

// Sets default values
AEnemyQueue::AEnemyQueue()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AEnemyQueue::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AEnemyQueue::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (CurrentEnemyCount < MaxAttackingEnemies) {
		if (WaitingEnemies.size() > 0) {
			AEnemyBase* enemy = WaitingEnemies.front();
			WaitingEnemies.pop();

			if (enemy->IsAlive()) {
				enemy->SetAttackState(true, true);
				CurrentEnemyCount++;
				AttackingEnemies.Emplace(enemy);
			}
		}
	}
}

void AEnemyQueue::AddToQueue(AEnemyBase* Enemy)
{
	WaitingEnemies.emplace(Enemy);
}

void AEnemyQueue::RemoveEnemy(AEnemyBase* Enemy)
{
	if (AttackingEnemies.Contains(Enemy)) {
		AttackingEnemies.Remove(Enemy);
		CurrentEnemyCount--;
	}
}

