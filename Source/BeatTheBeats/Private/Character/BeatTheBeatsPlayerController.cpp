// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/BeatTheBeatsPlayerController.h"
#include "EnhancedInputSubsystems.h"
#include "Engine/LocalPlayer.h"
#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"
#include "Beats/BeatManager.h"
#include "Score/ScoreManager.h"

void ABeatTheBeatsPlayerController::BeginPlay()
{
	Super::BeginPlay();

	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
	{
		Subsystem->AddMappingContext(ModifierMappingContext, 100);
		Subsystem->AddMappingContext(DefaultMappingContext, 0);
	}

	HUD = CreateWidget(this, HudClass);

	if (HUD) {
		HUD->AddToViewport();
	}

	BeatManager = Cast<ABeatManager>(UGameplayStatics::GetActorOfClass(this, BeatManagerClass));

	if (BeatManager == nullptr) {
		UE_LOG(LogTemp, Error, TEXT("No Beat Manager was found in the scene!"));
	}

	ScoreManager = Cast<AScoreManager>(UGameplayStatics::GetActorOfClass(this, ScoreManagerClass));

	if (ScoreManager == nullptr) {
		UE_LOG(LogTemp, Error, TEXT("No Score Manager was found in the scene!"));
	}
	else {
		ScoreManager->SetupScoreManager(HUD);
	}
}

void ABeatTheBeatsPlayerController::GameOver()
{
	GameOverScreen = CreateWidget(this, GameOverScreenClass);

	if (GameOverScreen) {
		GameOverScreen->AddToViewport();
	}
}