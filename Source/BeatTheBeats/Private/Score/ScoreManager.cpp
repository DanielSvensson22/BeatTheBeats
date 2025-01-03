// Fill out your copyright notice in the Description page of Project Settings.


#include "Score/ScoreManager.h"
#include "Kismet/GameplayStatics.h"
#include "Blueprint/UserWidget.h"
#include "Blueprint/WidgetTree.h"
#include "Components/TextBlock.h"
#include "Components/RadialSlider.h"
#include "Misc/Paths.h"
#include "Misc/FileHelper.h"

// Sets default values
AScoreManager::AScoreManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AScoreManager::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void AScoreManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (CurrentTimeUntilPointsLoss > 0) {
		CurrentTimeUntilPointsLoss -= DeltaTime;
	}
	else {
		if (!bHasLostPoints) {
			bHasLostPoints = true;
			AddPoints(-DamagePointsLoss);
		}
	}
}

void AScoreManager::SetupScoreManager(UUserWidget* UserWidget)
{
	UUserWidget* Widget = UserWidget;

	if (Widget != nullptr) {
		ScoreText = Cast<UTextBlock>(Widget->WidgetTree->FindWidget(TEXT("ScoreIndicator")));
		ScoreSlider = Cast<URadialSlider>(Widget->WidgetTree->FindWidget(TEXT("ScoreSlider")));

		if (ScoreText) {
			ScoreText->SetText(FText::FromString(TEXT("-")));
			ScoreText->SetColorAndOpacity(NoStageColor);
		}
		else {
			UE_LOG(LogTemp, Error, TEXT("Score Manager could not access score text!"));
		}

		if (ScoreSlider) {
			ScoreSlider->SetValue(1);
		}
		else {
			UE_LOG(LogTemp, Error, TEXT("Score Manager could not access score slider!"));
		}
	}
	else {
		UE_LOG(LogTemp, Error, TEXT("Score Manager could not access player HUD!"));
	}
}

void AScoreManager::AddPoints(float points)
{
	if (points > 0) {
		Points = FMath::Clamp(Points + points, 0, INT32_MAX);
	}
	
	CurrentStagePoints += points;

	while (CurrentStagePoints >= ScoreStages[CurrentStage].GetPointsNeeded() && CurrentStage < ScoreStages.Num() - 1) {
		CurrentStagePoints -= ScoreStages[CurrentStage].GetPointsNeeded();
		CurrentStage++;
	}

	while (CurrentStagePoints < 0 && CurrentStage > 0) {
		CurrentStage--;
		CurrentStagePoints = ScoreStages[CurrentStage].GetPointsNeeded() + CurrentStagePoints;
	}

	CurrentStagePoints = FMath::Clamp(CurrentStagePoints, 0, FMath::Abs(CurrentStagePoints));

	UpdateUI();
}

void AScoreManager::TookDamage()
{
	AddPoints(-DamagePointsLoss);
}

void AScoreManager::UpdateUI()
{
	if (ScoreText && ScoreSlider) {
		if (CurrentStage == 0 && CurrentStagePoints == 0) {
			ScoreText->SetText(FText::FromString(TEXT("-")));
			ScoreText->SetColorAndOpacity(NoStageColor);
			ScoreSlider->SetValue(1);
		}
		else {
			ScoreText->SetText(FText::FromString(ScoreStages[CurrentStage].GetStageIndicator()));
			ScoreText->SetColorAndOpacity(ScoreStages[CurrentStage].GetStageColor());
			ScoreSlider->SetValue(1 - (CurrentStagePoints / ScoreStages[CurrentStage].GetPointsNeeded()));
		}

		CurrentTimeUntilPointsLoss = TimeUntilPointsLoss;
		bHasLostPoints = false;
	}
}

void AScoreManager::Save()
{
	SaveEvent();
}

void AScoreManager::SaveEvent_Implementation()
{

}

void AScoreManager::SetPoints(int points)
{
	Points = (float)points;
}

/// <summary>
/// Gets save file path
/// Checks if save file directory exists and creates if not
/// Checks if save file exists and creates if not
/// </summary>
/// <returns>A file string with save file path</returns>
FString AScoreManager::GetSaveFilePath()
{
	IPlatformFile& _fileManager = FPlatformFileManager::Get().GetPlatformFile();
	FString _dir = FPaths::GameAgnosticSavedDir();
	_dir.Append(TEXT("Save"));

	//If directory doesn't exist, create directory
	if (!FPaths::DirectoryExists(_dir)) _fileManager.CreateDirectory(*_dir);
	
	//If save file doesn't exist, create save file
	_dir.Append(TEXT("/hiscore.json"));
	if (!_fileManager.FileExists(*_dir)) FFileHelper::SaveStringToFile((FString)"", *_dir, FFileHelper::EEncodingOptions::AutoDetect, &IFileManager::Get(), EFileWrite::FILEWRITE_Append);
	
	return _dir;
}

