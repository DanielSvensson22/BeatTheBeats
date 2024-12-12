// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/TricksterMelee.h"

float ATricksterMelee::ApplyDamage(float InitialDamage, Attacks AttackType, bool OnBeat, FVector HitLocation)
{
    float health = CurrentHealth;

	if (AttackType == Attacks::Attack_Guaranteed || AttackType == EnemyType) {
		bHitByOwnType = true;
	}
	else {
		bHitByOwnType = false;
	}

    float finalDamage = Super::ApplyDamage(InitialDamage, AttackType, OnBeat, HitLocation);

	if (bHitByOwnType) {
		return finalDamage;
	}

	CurrentHealth = health;

	return 0;
}

void ATricksterMelee::ApplyDamageEffects(float FinalDamage, bool OnBeat)
{
    if (bHitByOwnType) {
        Super::ApplyDamageEffects(FinalDamage, OnBeat);

		int rand = FMath::RandRange(0, 3);

		switch (rand) {
		case 0:
			if (EnemyType != Attacks::Attack_Neutral) {
				EnemyType = Attacks::Attack_Neutral;
			}
			else {
				EnemyType = Attacks::Attack_Type1;
			}

			SetEffectsColor(EnemyType);
			break;

		case 1:
			if (EnemyType != Attacks::Attack_Type1) {
				EnemyType = Attacks::Attack_Type1;
			}
			else {
				EnemyType = Attacks::Attack_Type2;
			}

			SetEffectsColor(EnemyType);
			break;

		case 2:
			if (EnemyType != Attacks::Attack_Type2) {
				EnemyType = Attacks::Attack_Type2;
			}
			else {
				EnemyType = Attacks::Attack_Type3;
			}

			SetEffectsColor(EnemyType);
			break;

		case 3:
			if (EnemyType != Attacks::Attack_Type3) {
				EnemyType = Attacks::Attack_Type3;
			}
			else {
				EnemyType = Attacks::Attack_Neutral;
			}

			SetEffectsColor(EnemyType);
			break;

		default:
			if (EnemyType != Attacks::Attack_Neutral) {
				EnemyType = Attacks::Attack_Neutral;
			}
			else {
				EnemyType = Attacks::Attack_Type1;
			}

			SetEffectsColor(EnemyType);
			break;
		}
    }
    else {
		SpawnDamageIndicator(TEXT("BLOCKED!"));
    }
}
