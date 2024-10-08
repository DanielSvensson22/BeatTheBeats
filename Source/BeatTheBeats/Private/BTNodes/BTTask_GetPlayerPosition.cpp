// Fill out your copyright notice in the Description page of Project Settings.


#include "BTNodes/BTTask_GetPlayerPosition.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Kismet/GameplayStatics.h"
#include "AIController.h"
#include "Enemy/EnemyBase.h"

UBTTask_GetPlayerPosition::UBTTask_GetPlayerPosition()
{
	NodeName = TEXT("Get Player Position");
}

EBTNodeResult::Type UBTTask_GetPlayerPosition::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);

	APawn* player = UGameplayStatics::GetPlayerPawn(this, 0);
	AEnemyBase* enemy = Cast<AEnemyBase>(OwnerComp.GetAIOwner()->GetPawn());

	if (player && enemy) {

		FVector dir = player->GetActorLocation() - enemy->GetActorLocation();

		float dot = enemy->GetActorForwardVector().Dot(dir);

		if (FVector::Dist(player->GetActorLocation(), enemy->GetActorLocation()) < enemy->GetMaxViewDistance()) {
			OwnerComp.GetBlackboardComponent()->SetValueAsObject(GetSelectedBlackboardKey(), player);
			return EBTNodeResult::Type::Succeeded;
		}
		else {
			OwnerComp.GetBlackboardComponent()->ClearValue(GetSelectedBlackboardKey());
			return EBTNodeResult::Type::Failed;
		}
	}

	return EBTNodeResult::Type::Failed;
}
