// Fill out your copyright notice in the Description page of Project Settings.


#include "BTNodes/BTTask_CheckAttackDistance.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Kismet/GameplayStatics.h"
#include "AIController.h"
#include "Enemy/EnemyBase.h"

UBTTask_CheckAttackDistance::UBTTask_CheckAttackDistance() {
	NodeName = TEXT("Check Attack Distance");
}

EBTNodeResult::Type UBTTask_CheckAttackDistance::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) {
	Super::ExecuteTask(OwnerComp, NodeMemory);

	APawn* player = UGameplayStatics::GetPlayerPawn(this, 0);
	AEnemyBase* enemy = Cast<AEnemyBase>(OwnerComp.GetAIOwner()->GetPawn());

	if (player && enemy) {
		if (FVector::Dist(player->GetActorLocation(), enemy->GetActorLocation()) <= enemy->GetMaxAttackDistance()) {
			return EBTNodeResult::Type::Succeeded;
		}
	}

	return EBTNodeResult::Type::Failed;
}