// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Combo.h"
#include "ComboConnection.generated.h"

/**
 * 
 */
USTRUCT(BlueprintType)
struct BEATTHEBEATS_API FComboConnection
{
	GENERATED_BODY()

public:
	FComboConnection();
	FComboConnection(Attacks connectingAttack, int fromComboStep, int toCombo);
	~FComboConnection();

	FORCEINLINE Attacks GetAttackWhichConnects() { return AttackWhichConnects; }
	FORCEINLINE int GetFromComboStep() { return FromComboStep; }
	FORCEINLINE int GetToCombo() { return ToCombo; }

private:

	UPROPERTY(EditDefaultsOnly)
	Attacks AttackWhichConnects = Attacks::Attack_Neutral;

	UPROPERTY(EditDefaultsOnly)
	int FromComboStep = 0;

	UPROPERTY(EditDefaultsOnly)
	int ToCombo = 0;
};
