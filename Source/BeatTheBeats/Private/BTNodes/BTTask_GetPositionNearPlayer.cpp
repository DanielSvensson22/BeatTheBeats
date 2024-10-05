// Fill out your copyright notice in the Description page of Project Settings.


#include "BTNodes/BTTask_GetPositionNearPlayer.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Kismet/GameplayStatics.h"
#include "AIController.h"
#include "Enemy/MeleeEnemy.h"
#include "Enemy/MeleeAIController.h"
#include "NavigationSystem.h"
#include "NavigationPath.h"

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

				if (controller->CurrentAngle >= 90 || controller->CurrentAngle <= -90) {
					controller->WalkingRight = !controller->WalkingRight;
				}

				bool Valid = GetValidPosition(controller);

				if (Valid) {
					Valid = GetValidPath(controller, enemy);
				}

				OwnerComp.GetBlackboardComponent()->SetValueAsVector(GetSelectedBlackboardKey(), controller->TargetPosition);
				OwnerComp.GetBlackboardComponent()->SetValueAsBool(BlackboardValidMoveKey.SelectedKeyName, Valid);
				controller->LastPosition = player->GetActorLocation();

				controller->CurrentAngle = FMath::Clamp(controller->CurrentAngle, -90, 90);

				return EBTNodeResult::Type::Succeeded;
			}
		}
	}	

	return EBTNodeResult::Type::Failed;
}

bool UBTTask_GetPositionNearPlayer::GetValidPosition(AMeleeAIController* controller)
{
	FVector Start = controller->GetPawn()->GetActorLocation();
	FVector End = controller->TargetPosition;
	UNavigationPath* NavPath = UNavigationSystemV1::FindPathToLocationSynchronously(controller->GetWorld(), Start, End, nullptr);

	if (NavPath == nullptr || !NavPath->IsValid() || NavPath->IsPartial()) {
		controller->WalkingRight = !controller->WalkingRight;
		return false;
	}

	return true;
}

void UBTTask_GetPositionNearPlayer::InitializeFromAsset(UBehaviorTree& Asset)
{
	Super::InitializeFromAsset(Asset);

	UBlackboardData* BBAsset = GetBlackboardAsset();
	if (BBAsset)
	{
		BlackboardValidMoveKey.ResolveSelectedKey(*BBAsset);
	}
	else
	{
		UE_LOG(LogBehaviorTree, Warning, TEXT("Can't initialize task: %s, make sure that behavior tree specifies blackboard asset!"), *GetName());
	}
}

bool UBTTask_GetPositionNearPlayer::GetValidPath(AMeleeAIController* controller, AEnemyBase* enemy)
{
	FHitResult result;
	FCollisionQueryParams params;

	params.AddIgnoredActor(enemy);

	if (GetWorld()->LineTraceSingleByChannel(result, enemy->GetActorLocation(), 
		FVector(controller->TargetPosition.X, controller->TargetPosition.Y, enemy->GetActorLocation().Z), ECollisionChannel::ECC_GameTraceChannel2, params)) {
		controller->WalkingRight = !controller->WalkingRight;
		return false;
	}

	return true;
}
