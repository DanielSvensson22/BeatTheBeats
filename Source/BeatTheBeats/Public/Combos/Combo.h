// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimationAsset.h"
#include "GenericPlatform/GenericPlatform.h"
#include "QTEDescription.h"
#include "Combo.generated.h"

UENUM(BlueprintType)
enum class ComboEffect : uint8 {
	None = 0,
	Special1 = 1,
	Special2 = 2,
	Special3 = 3
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
	FCombo(TArray<UAnimMontage*> animations, TArray<Attacks>& attacks, TArray<float>& motionValues);
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

	FORCEINLINE int GetNextAttack(int current) const {
		int next = current + 1;

		if (next >= ComboAttacks.Num()) {
			next -= ComboAttacks.Num();
		}

		return next;
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

	FORCEINLINE UAnimMontage* GetCurrentMontage() {
		if (CurrentAttack == -1) {
			return nullptr;
		}

		return Animations[CurrentAttack];
	}

	FORCEINLINE UAnimMontage* GetAnimMontage(int index) { 
		int next = index;

		if (index >= Animations.Num()) {
			next = 0;
		}

		return Animations[next];
	}

	FORCEINLINE FText& GetName() { return ComboName; }

	FORCEINLINE bool HasQTE() {
		return QTEDescription.Num() > 0;
	}

	FORCEINLINE TArray<FQTEDescription>* GetQTEDescription() {
		return &QTEDescription;
	}

	FORCEINLINE ComboEffect GetComboEffect() {
		return ComboBonus;
	}

private:

	UPROPERTY(EditDefaultsOnly)
	TArray<UAnimMontage*> Animations;

	UPROPERTY(EditDefaultsOnly)
	TArray<Attacks> ComboAttacks;

	UPROPERTY(EditDefaultsOnly)
	TArray<float> MotionValues;

	int CurrentAttack = -1;

	UPROPERTY(EditDefaultsOnly)
	FText ComboName;

	UPROPERTY(EditDefaultsOnly)
	TArray<FQTEDescription> QTEDescription;

	UPROPERTY(EditDefaultsOnly)
	ComboEffect ComboBonus;
};
