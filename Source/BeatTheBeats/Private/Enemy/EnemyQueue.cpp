// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/EnemyQueue.h"
#include "Enemy/EnemyBase.h"
#include "NavigationSystem.h"

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
	
	UNavigationSystemV1* NavSystem = UNavigationSystemV1::GetCurrent(GetWorld());

	if (NavSystem) {
		NavSystem->Build();
	}
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

	if (CurrentRangedEnemyCount < MaxRangedEnemies) {
		if (WaitingRangedEnemies.size() > 0) {
			AEnemyBase* enemy = WaitingRangedEnemies.front();
			WaitingRangedEnemies.pop();

			if (enemy->IsAlive()) {
				enemy->SetAttackState(true, true);
				CurrentRangedEnemyCount++;
				AttackingEnemies.Emplace(enemy);
			}
		}
	}
}

void AEnemyQueue::AddToQueue(AEnemyBase* Enemy)
{
	WaitingEnemies.emplace(Enemy);
}

void AEnemyQueue::AddToRangedQueue(AEnemyBase* Enemy)
{
	WaitingRangedEnemies.emplace(Enemy);
}

void AEnemyQueue::RemoveEnemy(AEnemyBase* Enemy, bool IsMelee)
{
	if (AttackingEnemies.Contains(Enemy)) {
		AttackingEnemies.Remove(Enemy);

		if (IsMelee) {
			CurrentEnemyCount--;
		}
		else {
			CurrentRangedEnemyCount--;
		}	
	}
}

