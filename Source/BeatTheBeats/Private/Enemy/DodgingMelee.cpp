// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/DodgingMelee.h"

float ADodgingMelee::ApplyDamage(float InitialDamage, Attacks AttackType, bool OnBeat, FVector HitLocation)
{
	float health = CurrentHealth;

	if (AttackType == Attacks::Attack_Guaranteed) {
		bGotHitOnBeat = true;
	}
	else {
		bGotHitOnBeat = OnBeat;
	}

	float finalDamage = Super::ApplyDamage(InitialDamage, AttackType, OnBeat, HitLocation);

	if (bGotHitOnBeat) {
		return finalDamage;
	}

	CurrentHealth = health;

	return InitialDamage * 10;
}

void ADodgingMelee::ApplyDamageEffects(float FinalDamage, bool OnBeat)
{
	if (bGotHitOnBeat) {
		Super::ApplyDamageEffects(FinalDamage, OnBeat);
	}
	else {
		SpawnDamageIndicator(TEXT("DODGED!!!"));
	}
}
