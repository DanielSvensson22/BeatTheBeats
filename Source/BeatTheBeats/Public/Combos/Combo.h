// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimationAsset.h"
#include "GenericPlatform/GenericPlatform.h"
#include "Combo.generated.h"

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

	FORCEINLINE float GetAnimLength(int currentAttack) const { 
		if (Animations[currentAttack] == nullptr) {
			return -1;
		}

		return Animations[currentAttack]->GetPlayLength(); }

	FORCEINLINE float GetCurrentAnimLength() const { 
		if (Animations[CurrentAttack] == nullptr) {
			return -1;
		}

		return Animations[CurrentAttack]->GetPlayLength(); 
	}

	FORCEINLINE float GetMotionValue(int currentAttack) const { return MotionValues[currentAttack]; }
	FORCEINLINE float GetCurrentMotionValue() const { return MotionValues[CurrentAttack]; }

	FORCEINLINE Attacks GetAttackType(int index) const {
		if (index >= ComboAttacks.Num()) {
			return Attacks::Attack_NONE;
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

	FORCEINLINE int AttackCount() const { return ComboAttacks.Num(); }

private:

	UPROPERTY(EditDefaultsOnly)
	TArray<UAnimationAsset*> Animations;

	UPROPERTY(EditDefaultsOnly)
	TArray<Attacks> ComboAttacks;

	UPROPERTY(EditDefaultsOnly)
	TArray<float> MotionValues;

	int CurrentAttack = -1;
};
