// Fill out your copyright notice in the Description page of Project Settings.


#include "BTNodes/BTTask_SetEnemyFocus.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AIController.h"
#include "Kismet/GameplayStatics.h"
#include "Enemy/CrowdHandlingAIController.h"
#include "Enemy/EnemyBase.h"

UBTTask_SetEnemyFocus::UBTTask_SetEnemyFocus()
{
	NodeName = TEXT("Set Enemy Focus");
}

EBTNodeResult::Type UBTTask_SetEnemyFocus::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	ACrowdHandlingAIController* controller = Cast<ACrowdHandlingAIController>(OwnerComp.GetAIOwner());

	if (GetSelectedBlackboardKey() == "Focus") {
		controller->SetFocusState(UGameplayStatics::GetPlayerPawn(this, 0), true);
	}
	else {
		controller->SetFocusState(UGameplayStatics::GetPlayerPawn(this, 0), false);

		AEnemyBase* enemy = Cast<AEnemyBase>(OwnerComp.GetAIOwner()->GetPawn());

		if (enemy) {
			enemy->ExitQueue();
		}
	}

	return EBTNodeResult::Type::Succeeded;
}
