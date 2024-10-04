// Fill out your copyright notice in the Description page of Project Settings.


#include "BTNodes/BTTaskNode_ShouldFlee.h"
#include "Kismet/GameplayStatics.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AIController.h"
#include "Enemy/RangedEnemy.h"
#include "NavigationSystem.h"

UBTTaskNode_ShouldFlee::UBTTaskNode_ShouldFlee()
{
	NodeName = TEXT("Should Flee?");
}

EBTNodeResult::Type UBTTaskNode_ShouldFlee::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	APawn* player = UGameplayStatics::GetPlayerPawn(this, 0);
	ARangedEnemy* enemy = Cast<ARangedEnemy>(OwnerComp.GetAIOwner()->GetPawn());

	if (player && enemy) {
		if (FVector::Dist(enemy->GetActorLocation(), player->GetActorLocation()) < enemy->GetFleeDistance()) {
			UNavigationSystemV1* NavSystem = UNavigationSystemV1::GetCurrent(player);

			FVector FleeLocation;
			FNavLocation location;

			FVector Back = player->GetActorLocation() + (-player->GetActorForwardVector() * enemy->GetMaxAttackDistance());
			FVector Right = player->GetActorLocation() + (player->GetActorRightVector() * enemy->GetMaxAttackDistance());
			
			if (NavSystem->GetRandomReachablePointInRadius(Back, 100, location)) {
				FleeLocation = location.Location;
			}
			else if (NavSystem->GetRandomReachablePointInRadius(Right, 100, location)) {
				FleeLocation = location.Location;
			}
			else {
				NavSystem->GetRandomReachablePointInRadius(player->GetActorLocation(), enemy->GetMaxAttackDistance(), location);

				FleeLocation = location.Location;
			}

			OwnerComp.GetBlackboardComponent()->SetValueAsVector(GetSelectedBlackboardKey(), FleeLocation);

			return EBTNodeResult::Type::Succeeded;
		}
	}

	return EBTNodeResult::Type::Failed;
}
