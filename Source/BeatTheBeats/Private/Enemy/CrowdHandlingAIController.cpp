// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/CrowdHandlingAIController.h"
#include "Navigation/CrowdFollowingComponent.h"

ACrowdHandlingAIController::ACrowdHandlingAIController(const FObjectInitializer& ObjectInitializer) :
	Super(ObjectInitializer.SetDefaultSubobjectClass<UCrowdFollowingComponent>(TEXT("PathFollowingComponent")))
{

}

void ACrowdHandlingAIController::BeginPlay()
{
	Super::BeginPlay();

	UCrowdFollowingComponent* CrowdComponent = FindComponentByClass<UCrowdFollowingComponent>();

	if (CrowdComponent) {
		CrowdComponent->SetCrowdSeparation(true);
		CrowdComponent->SetCrowdSeparationWeight(50.0f);
		CrowdComponent->SetCrowdAvoidanceRangeMultiplier(1.1f);
		CrowdComponent->SetCrowdAvoidanceQuality(ECrowdAvoidanceQuality::Good);
	}
	else {
		UE_LOG(LogTemp, Error, TEXT("AI controller had no crowd following component!"));
	}
}
