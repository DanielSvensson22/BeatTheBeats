// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Combo.h"
#include "QTEDescription.generated.h"

/**
 * 
 */
USTRUCT(BlueprintType)
struct BEATTHEBEATS_API FQTEDescription
{
	GENERATED_BODY()

public:
	FQTEDescription();
	FQTEDescription(Attacks attack, float beatTimeDivisor, FVector2D offsetRange);
	~FQTEDescription();

	FORCEINLINE Attacks GetAttack() { return Attack; }
	FORCEINLINE float GetBeatTimeDivisor() { return BeatTimeDivisor; }
	FORCEINLINE FVector2D GetOffsetRange() { return OffsetRange; }

private:

	UPROPERTY(EditDefaultsOnly)
	Attacks Attack;

	UPROPERTY(EditDefaultsOnly)
	float BeatTimeDivisor = 1.0f;

	UPROPERTY(EditDefaultsOnly)
	FVector2D OffsetRange;
};
