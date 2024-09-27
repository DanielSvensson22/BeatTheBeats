// Fill out your copyright notice in the Description page of Project Settings.


#include "Combos/Combo.h"

FCombo::FCombo()
{
}

FCombo::FCombo(TArray<UAnimationAsset*> animations, TArray<Attacks>& attacks, TArray<float>& motionValues) : Animations(animations), ComboAttacks(attacks), MotionValues(motionValues)
{
}

FCombo::~FCombo()
{
}
