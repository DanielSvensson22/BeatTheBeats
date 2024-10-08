// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/QTEComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Character/BeatTheBeatsPlayerController.h"
#include "Beats/BeatManager.h"
#include "Blueprint/UserWidget.h"
#include "Blueprint/WidgetTree.h"
#include "Components/Image.h"

// Sets default values for this component's properties
UQTEComponent::UQTEComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UQTEComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...

	ABeatTheBeatsPlayerController* controller = Cast<ABeatTheBeatsPlayerController>(GetOwner()->GetInstigatorController());
	
	if (controller) {
		Widget = controller->GetHUD();
	}

	if (Widget == nullptr) {
		UE_LOG(LogTemp, Error, TEXT("QTE Component could not access player controller!"));
	}
	else {
		AttackIndicator = Cast<UImage>(Widget->WidgetTree->FindWidget(TEXT("QTEImage")));

		AttackCircle = Cast<UImage>(Widget->WidgetTree->FindWidget(TEXT("QTECircle")));

		if (AttackIndicator == nullptr) {
			UE_LOG(LogTemp, Error, TEXT("QTEImage was not found on widget!"));
		}
		else {
			StartPos = AttackIndicator->GetRenderTransform().Translation;
			AttackIndicator->SetIsEnabled(false);
			AttackIndicator->SetVisibility(ESlateVisibility::Hidden);
		}

		if (AttackCircle == nullptr) {
			UE_LOG(LogTemp, Error, TEXT("QTECircle was not found on widget!"));
		}
		else {
			AttackCircle->SetIsEnabled(false);
			AttackCircle->SetVisibility(ESlateVisibility::Hidden);
		}
	}

	BeatManager = Cast<ABeatManager>(UGameplayStatics::GetActorOfClass(this, BeatManagerClass));

	if (BeatManager == nullptr) {
		UE_LOG(LogTemp, Error, TEXT("No Beat Manager was found in the scene!"));
	}
}


// Called every frame
void UQTEComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...

	if (bIsActive) {
		CurrentTimeStep += DeltaTime / UGameplayStatics::GetGlobalTimeDilation(this) * TimeStepMultiplier;

		if (CurrentTimeStep > BeatManager->TimeBetweenBeats() / (*CurrentQTE)[CurrentQTEStep].GetBeatTimeDivisor()) {
			EndQTE();
		}

		if (AttackCircle) {
			AttackCircle->SetRenderScale(FVector2D(BeatManager->GetIndicatorScale(MaxCircleScale, MinCircleScale, CurrentTimeStep)));
		}
	}
}

void UQTEComponent::StartQTE(TArray<FQTEDescription>* qte)
{
	CurrentQTE = qte;

	UGameplayStatics::SetGlobalTimeDilation(this, 0.01f);
	CurrentTimeStep = 0;
	CurrentQTEStep = 0;

	bIsActive = true;

	AttackIndicator->SetIsEnabled(true);
	AttackCircle->SetIsEnabled(true);
	AttackIndicator->SetVisibility(ESlateVisibility::Visible);
	AttackCircle->SetVisibility(ESlateVisibility::Visible);

	UpdateWidgetIndicators(0);

	UE_LOG(LogTemp, Display, TEXT("Started QTE"));
}

void UQTEComponent::AttemptAttack(Attacks Attack)
{
	FQTEDescription& desc = (*CurrentQTE)[CurrentQTEStep];

	if (CurrentTimeStep / BeatManager->TimeBetweenBeats() > 0.9) {
		if (Attack == desc.GetAttack()) {
			if (CurrentQTEStep < (*CurrentQTE).Num() - 1) {
				CurrentQTEStep++;
				CurrentTimeStep = 0;

				UpdateWidgetIndicators(CurrentQTEStep);
			}
			else {
				// To do: Add successful qte effect.
				UE_LOG(LogTemp, Warning, TEXT("QTE was successfull!"));
				EndQTE();
			}
		}
		else {
			EndQTE();
		}
	}
	else {
		EndQTE();
	}
}

void UQTEComponent::EndQTE()
{
	UGameplayStatics::SetGlobalTimeDilation(this, 1);
	bIsActive = false;

	AttackIndicator->SetIsEnabled(false);
	AttackCircle->SetIsEnabled(false);
	AttackIndicator->SetVisibility(ESlateVisibility::Hidden);
	AttackCircle->SetVisibility(ESlateVisibility::Hidden);

	UE_LOG(LogTemp, Display, TEXT("Ended QTE"));
}

void UQTEComponent::UpdateWidgetIndicators(int index)
{
	FQTEDescription& desc = (*CurrentQTE)[index];

	switch (desc.GetAttack()) {
	case Attacks::Attack_Neutral:
		AttackIndicator->SetBrushTintColor(NeutralColor);
		break;

	case Attacks::Attack_Type1:
		AttackIndicator->SetBrushTintColor(Attack1Color);
		break;

	case Attacks::Attack_Type2:
		AttackIndicator->SetBrushTintColor(Attack2Color);
		break;

	case Attacks::Attack_Type3:
		AttackIndicator->SetBrushTintColor(Attack3Color);
		break;

	default:
		AttackIndicator->SetBrushTintColor(NeutralColor);
		break;
	}

	AttackIndicator->SetRenderTranslation(StartPos + FVector2D(FMath::RandRange(-desc.GetOffsetRange().X, desc.GetOffsetRange().X), FMath::RandRange(-desc.GetOffsetRange().Y, desc.GetOffsetRange().Y)));
	AttackCircle->SetRenderTranslation(AttackIndicator->GetRenderTransform().Translation);

	AttackIndicator->SetRenderScale(FVector2D(BeatManager->GetIndicatorScale(MaxCircleScale, MinCircleScale, BeatManager->TimeBetweenBeats() / desc.GetBeatTimeDivisor())));
}

