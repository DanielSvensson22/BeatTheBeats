// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ScoreStage.h"
#include "ScoreManager.generated.h"

class UTextBlock;
class URadialSlider;
class UUserWidget;

UCLASS()
class BEATTHEBEATS_API AScoreManager : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AScoreManager();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void SetupScoreManager(UUserWidget* UserWidget);

	UFUNCTION(BlueprintPure)
	FORCEINLINE int GetPoints() const { return (int)Points; }

	UFUNCTION(BlueprintCallable)
	void SetPoints(int points);

	//Get the file path for the .json save file
	UFUNCTION(BlueprintPure)
	FString GetSaveFilePath();

	void AddPoints(float points);

	void TookDamage();

	void UpdateUI();

	void Save();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "SaveEvent")
	void SaveEvent();

private:

	UPROPERTY(VisibleAnywhere)
	float Points = 0;

	float CurrentStagePoints = 0;

	FString FilePath;

	UPROPERTY(EditDefaultsOnly)
	TArray<FScoreStage> ScoreStages;

	int CurrentStage = 0;

	UPROPERTY(EditDefaultsOnly)
	float DamagePointsLoss = 50;

	UPROPERTY(EditDefaultsOnly)
	float TimeUntilPointsLoss = 5;

	float CurrentTimeUntilPointsLoss;

	bool bHasLostPoints = false;

	UTextBlock* ScoreText;
	URadialSlider* ScoreSlider;

	UPROPERTY(EditDefaultsOnly)
	FLinearColor NoStageColor;
};
