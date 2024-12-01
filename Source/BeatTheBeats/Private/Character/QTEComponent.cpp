// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/QTEComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Character/BeatTheBeatsPlayerController.h"
#include "Character/PlayerCharacter.h"
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

		player = Cast<APlayerCharacter>(controller->GetPawn());

		if (player == nullptr) {
			UE_LOG(LogTemp, Error, TEXT("QTE Component could not access player character!"));
		}
	}

	if (Widget == nullptr) {
		UE_LOG(LogTemp, Error, TEXT("QTE Component could not access player controller!"));
	}
	else {
		AttackIndicator = Cast<UImage>(Widget->WidgetTree->FindWidget(TEXT("QTEImage")));

		AttackCircle = Cast<UImage>(Widget->WidgetTree->FindWidget(TEXT("QTECircle")));

		MinClosenessIndicator = Cast<UImage>(Widget->WidgetTree->FindWidget(TEXT("QTELeewayCircle")));

		if (AttackIndicator == nullptr) {
			UE_LOG(LogTemp, Error, TEXT("QTEImage was not found on widget!"));
		}
		else {
			StartPos = AttackIndicator->GetRenderTransform().Translation;
			AttackIndicator->SetBrushTintColor(FLinearColor::Black);
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

		if (MinClosenessIndicator == nullptr) {
			UE_LOG(LogTemp, Error, TEXT("QTELeewayCircle was not found on widget!"));
		}
		else {
			MinClosenessIndicator->SetIsEnabled(false);
			MinClosenessIndicator->SetVisibility(ESlateVisibility::Hidden);
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
		CurrentTimeStep += DeltaTime / UGameplayStatics::GetGlobalTimeDilation(this) * TimeStepMultiplier * SpeedIncrease;

		if (CurrentTimeStep > BeatManager->TimeBetweenBeats() / (*CurrentQTE)[CurrentQTEStep].GetBeatTimeDivisor()) {
			EndQTE();
		}

		if (AttackCircle && CurrentQTE) {
			AttackCircle->SetRenderScale(FVector2D(BeatManager->GetIndicatorScale(MaxCircleScale, MinCircleScale, CurrentTimeStep * (*CurrentQTE)[CurrentQTEStep].GetBeatTimeDivisor())));
		}
	}
}

void UQTEComponent::StartQTE(TArray<FQTEDescription>* qte, ComboEffect effect)
{
	CurrentQTE = qte;
	CurrentComboEffect = effect;

	UGameplayStatics::SetGlobalTimeDilation(this, 0.01f);
	CurrentTimeStep = 0;
	CurrentQTEStep = 0;

	if (CurrentComboEffect != ComboEffect::ExtraSpecial2) {
		SpeedIncrease = 1;
	}

	bIsActive = true;

	AttackIndicator->SetIsEnabled(true);
	AttackCircle->SetIsEnabled(true);
	MinClosenessIndicator->SetIsEnabled(true);
	AttackIndicator->SetVisibility(ESlateVisibility::Visible);
	AttackCircle->SetVisibility(ESlateVisibility::Visible);
	MinClosenessIndicator->SetVisibility(ESlateVisibility::Visible);

	UpdateWidgetIndicators(0);

	UE_LOG(LogTemp, Display, TEXT("Started QTE"));
}

void UQTEComponent::AttemptAttack(Attacks Attack)
{
	FQTEDescription& desc = (*CurrentQTE)[CurrentQTEStep];

	if (CurrentTimeStep / (BeatManager->TimeBetweenBeats() / (*CurrentQTE)[CurrentQTEStep].GetBeatTimeDivisor()) > MinClosenessToBeat) {
		if (Attack == desc.GetAttack()) {
			if (CurrentQTEStep < (*CurrentQTE).Num() - 1) {
				CurrentQTEStep++;
				CurrentTimeStep = 0;

				UpdateWidgetIndicators(CurrentQTEStep);
			}
			else {
				
				if (CurrentComboEffect != ComboEffect::None) {
					
					switch (CurrentComboEffect) {
					case ComboEffect::Special1:
						player->Special1();
						break;

					case ComboEffect::Special2:
						player->Special2();
						break;

					case ComboEffect::Special3:
						player->Special3();
						break;

					case ComboEffect::ExtraSpecial2:
						player->Special2();
						break;

					default:
						player->Special1();
						break;
					}
				}

				CurrentQTE = nullptr;

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

	if (CurrentQTE && CurrentComboEffect != ComboEffect::ExtraSpecial2) {
		player->FailedSpecial();
	}

	AttackIndicator->SetIsEnabled(false);
	AttackCircle->SetIsEnabled(false);
	MinClosenessIndicator->SetIsEnabled(false);
	AttackIndicator->SetVisibility(ESlateVisibility::Hidden);
	AttackCircle->SetVisibility(ESlateVisibility::Hidden);
	MinClosenessIndicator->SetVisibility(ESlateVisibility::Hidden);

	CurrentComboEffect = ComboEffect::None;
	CurrentQTE = nullptr;

	if (player) {
		player->ExitQTE();
	}

	UE_LOG(LogTemp, Display, TEXT("Ended QTE"));
}

void UQTEComponent::AddSpeed(float speed)
{
	SpeedIncrease += speed;
}

void UQTEComponent::UpdateWidgetIndicators(int index)
{
	FQTEDescription& desc = (*CurrentQTE)[index];

	Attacks attack = desc.GetAttack();

	if (desc.IsRandom()) {
		int rand = FMath::RandRange(0, 3);

		switch (rand) {
		case 0:
			attack = Attacks::Attack_Neutral;
			break;

		case 1:
			attack = Attacks::Attack_Type1;
			break;

		case 2:
			attack = Attacks::Attack_Type2;
			break;

		case 3:
			attack = Attacks::Attack_Type3;
			break;
		}

		desc.SetAttack(attack);
	}

	switch (attack) {
	case Attacks::Attack_Neutral:
		MinClosenessIndicator->SetBrushTintColor(NeutralColor);
		break;

	case Attacks::Attack_Type1:
		MinClosenessIndicator->SetBrushTintColor(Attack1Color);
		break;

	case Attacks::Attack_Type2:
		MinClosenessIndicator->SetBrushTintColor(Attack2Color);
		break;

	case Attacks::Attack_Type3:
		MinClosenessIndicator->SetBrushTintColor(Attack3Color);
		break;

	default:
		MinClosenessIndicator->SetBrushTintColor(NeutralColor);
		break;
	}

	FVector2D RandStart = StartPos + FVector2D(FMath::RandRange(-desc.GetOffsetRange().X, desc.GetOffsetRange().X),
												FMath::RandRange(-desc.GetOffsetRange().Y, desc.GetOffsetRange().Y));

	AttackIndicator->SetRenderTranslation(RandStart);
	MinClosenessIndicator->SetRenderTranslation(RandStart);
	AttackCircle->SetRenderTranslation(AttackIndicator->GetRenderTransform().Translation);

	FVector2D scale = FVector2D(BeatManager->GetIndicatorScale(MaxCircleScale, MinCircleScale, BeatManager->TimeBetweenBeats()));

	AttackIndicator->SetRenderScale(scale);
	MinClosenessIndicator->SetRenderScale(scale / MinClosenessToBeat);
}

