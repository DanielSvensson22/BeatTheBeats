// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Math/Color.h"
#include "ScoreStage.generated.h"

/**
 * 
 */
USTRUCT(BlueprintType)
struct BEATTHEBEATS_API FScoreStage
{
	GENERATED_BODY()

public:
	FScoreStage();
	FScoreStage(FLinearColor color, FString indicator, float points);
	~FScoreStage();

	FORCEINLINE FLinearColor GetStageColor() const { return StageColor; }
	FORCEINLINE FString GetStageIndicator() const { return StageIndicator; }
	FORCEINLINE float GetPointsNeeded() const { return PointsNeeded; }

private:

	UPROPERTY(EditDefaultsOnly)
	FLinearColor StageColor = FLinearColor::Black;

	UPROPERTY(EditDefaultsOnly)
	FString StageIndicator;

	UPROPERTY(EditDefaultsOnly)
	float PointsNeeded = 0;
};
