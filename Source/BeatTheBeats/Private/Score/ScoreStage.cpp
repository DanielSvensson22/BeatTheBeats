// Fill out your copyright notice in the Description page of Project Settings.


#include "Score/ScoreStage.h"

FScoreStage::FScoreStage()
{
}

FScoreStage::FScoreStage(FLinearColor color, FString indicator, float points) : StageColor(color), StageIndicator(indicator), PointsNeeded(points)
{
}

FScoreStage::~FScoreStage()
{
}
