// Fill out your copyright notice in the Description page of Project Settings.


#include "BTNodes/BTTask_GetPositionNearPlayer.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Kismet/GameplayStatics.h"
#include "AIController.h"
#include "Enemy/MeleeEnemy.h"
#include "Enemy/MeleeAIController.h"
#include "NavigationSystem.h"

UBTTask_GetPositionNearPlayer::UBTTask_GetPositionNearPlayer()
{
	NodeName = TEXT("Get Position Near Player");
}

EBTNodeResult::Type UBTTask_GetPositionNearPlayer::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AMeleeAIController* controller = Cast<AMeleeAIController>(OwnerComp.GetAIOwner());

	if (controller) {
		if (controller->RandomDistance == 0) {
			AMeleeEnemy* enemy = Cast<AMeleeEnemy>(OwnerComp.GetAIOwner()->GetPawn());

			controller->RandomDistance = FMath::RandRange(enemy->GetMinStalkDistance(), enemy->GetMaxStalkDistance());

			controller->WalkingRight = FMath::RandBool();

			return EBTNodeResult::Type::Succeeded;
		}
		else {
			if (controller->TargetPosition == FVector::ZeroVector) {
				APawn* player = UGameplayStatics::GetPlayerPawn(this, 0);

				controller->TargetPosition = player->GetActorLocation() + controller->RandomDistance * player->GetActorForwardVector() * FVector(1, 1, 0);

				GetValidPosition(controller);

				controller->CurrentAngle = 0;

				OwnerComp.GetBlackboardComponent()->SetValueAsVector(GetSelectedBlackboardKey(), controller->TargetPosition);

				return EBTNodeResult::Type::Succeeded;
			}
			else {
				APawn* player = UGameplayStatics::GetPlayerPawn(this, 0);
				AMeleeEnemy* enemy = Cast<AMeleeEnemy>(OwnerComp.GetAIOwner()->GetPawn());

				float dir = 1;

				if (!controller->WalkingRight) {
					dir = -1;
				}

				controller->CurrentAngle += dir * enemy->GetStalkSpeed();

				controller->TargetPosition = player->GetActorLocation() +
											(controller->RandomDistance * player->GetActorForwardVector().RotateAngleAxis(controller->CurrentAngle, FVector::UpVector));

				if (controller->CurrentAngle > 90 || controller->CurrentAngle < -90) {
					controller->WalkingRight = !controller->WalkingRight;
				}

				GetValidPosition(controller);

				OwnerComp.GetBlackboardComponent()->SetValueAsVector(GetSelectedBlackboardKey(), controller->TargetPosition);

				return EBTNodeResult::Type::Succeeded;
			}
		}
	}	

	return EBTNodeResult::Type::Failed;
}

void UBTTask_GetPositionNearPlayer::GetValidPosition(AMeleeAIController* controller)
{
	auto* const NavSystem = UNavigationSystemV1::GetCurrent(GetWorld());
	FNavLocation location;
	NavSystem->ProjectPointToNavigation(controller->TargetPosition, location);
	controller->TargetPosition = location.Location;
}
