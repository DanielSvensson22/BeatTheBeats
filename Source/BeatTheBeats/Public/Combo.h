// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimationAsset.h"
#include "GenericPlatform/GenericPlatform.h"
#include "Combo.generated.h"

UENUM(BlueprintType)
enum class Attacks : uint8 {
	LightAttack = 0,
	HeavyAttack = 1,
	Attacks_MAX UMETA(Hidden)
};

/**
 * 
 */
USTRUCT(BlueprintType)
struct BEATTHEBEATS_API FCombo
{
	GENERATED_BODY()

public:
	FCombo();
	FCombo(TArray<UAnimationAsset*> animations, TArray<Attacks>& attacks, TArray<float>& motionValues);
	~FCombo();

	FORCEINLINE int NextAttack() {
		CurrentAttack++;

		if (CurrentAttack >= ComboAttacks.Num()) {
			CurrentAttack = 0;
		}

		return CurrentAttack;
	}

	FORCEINLINE int NextAttack(int current) {
		CurrentAttack = current + 1;

		if (CurrentAttack >= ComboAttacks.Num()) {
			CurrentAttack = 0;
		}

		return CurrentAttack;
	}

	FORCEINLINE int ResetCombo() { CurrentAttack = -1; return -1; }

	FORCEINLINE float GetAnimLength(int currentAttack) const { return Animations[currentAttack]->GetPlayLength(); }
	FORCEINLINE float GetCurrentAnimLength() const { return Animations[CurrentAttack]->GetPlayLength(); }

	FORCEINLINE float GetMotionValue(int currentAttack) const { return MotionValues[currentAttack]; }
	FORCEINLINE float GetCurrentMotionValue() const { return MotionValues[CurrentAttack]; }

	FORCEINLINE Attacks GetAttackType(int index) const {
		if (index >= ComboAttacks.Num()) {
			return Attacks::Attacks_MAX;
		}

		return ComboAttacks[index];
	}

	FORCEINLINE bool AttackMatchesNextAttack(Attacks attack, int next) const { 
		int nextAttack = next;
		if (nextAttack >= ComboAttacks.Num()) {
			nextAttack = 0;
		}

		if (attack == ComboAttacks[nextAttack]) {
			return true;
		}

		return false;
	}

private:

	UPROPERTY(EditDefaultsOnly)
	TArray<UAnimationAsset*> Animations;

	UPROPERTY(EditDefaultsOnly)
	TArray<Attacks> ComboAttacks;

	UPROPERTY(EditDefaultsOnly)
	TArray<float> MotionValues;

	int CurrentAttack = -1;
};
