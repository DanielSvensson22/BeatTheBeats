// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "TutorialChecklist.generated.h"

/**
 * 
 */
UCLASS()
class BEATTHEBEATS_API UTutorialChecklist : public UObject
{
	GENERATED_BODY()
	
public:
	


public:
	bool bDodge = false;
	bool bAttack1 = false;
	bool bAttack2 = false;
	bool bAttack3 = false;
	bool bAttackNeutral = false;
	bool bBlock1 = false;
	bool bBlock2 = false;
	bool bBlock3 = false;
	bool bBlockNeutral = false;
	bool Combo1111 = false; // Combo when doing attack1 four times
	bool Combo2222 = false; // Combo when doing attack2 four times
	bool Combo3333 = false; // Combo when doing attack3 four times
	bool Combo1230 = false; // 0 is AttackNeutral
	bool Combo1323 = false;
};
