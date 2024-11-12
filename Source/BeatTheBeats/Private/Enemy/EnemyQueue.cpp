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
		if (WaitingEnemies.Num() > 0) {
			AEnemyBase* enemy = WaitingEnemies[0];
			WaitingEnemies.RemoveAt(0);

			if (enemy->IsAlive()) {
				enemy->SetAttackState(true, true);
				CurrentEnemyCount++;
				AttackingEnemies.Emplace(enemy);
			}
		}
	}

	if (CurrentRangedEnemyCount < MaxRangedEnemies) {
		if (WaitingRangedEnemies.Num() > 0) {
			AEnemyBase* enemy = WaitingRangedEnemies[0];
			WaitingRangedEnemies.RemoveAt(0);

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
	if (WaitingEnemies.Contains(Enemy)) {
		return;
	}

	WaitingEnemies.Emplace(Enemy);
}

void AEnemyQueue::AddToRangedQueue(AEnemyBase* Enemy)
{
	if (WaitingRangedEnemies.Contains(Enemy)) {
		return;
	}

	WaitingRangedEnemies.Emplace(Enemy);
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

