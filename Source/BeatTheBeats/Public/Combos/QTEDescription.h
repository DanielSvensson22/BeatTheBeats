// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "QTEDescription.generated.h"

UENUM(BlueprintType)
enum class Attacks : uint8 {
	Attack_Neutral = 0,
	Attack_Type1 = 1,
	Attack_Type2 = 2,
	Attack_Type3 = 3,
	Attack_Pause = 4,

	Attack_NONE UMETA(Hidden)
};

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
	FORCEINLINE bool IsRandom() { return bIsRandom; }

	void SetAttack(Attacks attack);

private:

	UPROPERTY(EditDefaultsOnly)
	Attacks Attack;

	UPROPERTY(EditDefaultsOnly)
	float BeatTimeDivisor = 1.0f;

	UPROPERTY(EditDefaultsOnly)
	FVector2D OffsetRange;

	UPROPERTY(EditDefaultsOnly)
	bool bIsRandom = false;
};
