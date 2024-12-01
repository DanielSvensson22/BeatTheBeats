// Fill out your copyright notice in the Description page of Project Settings.


#include "Combos/QTEDescription.h"

FQTEDescription::FQTEDescription()
{
}

FQTEDescription::FQTEDescription(Attacks attack, float beatTimeDivisor, FVector2D offsetRange) : Attack(attack), BeatTimeDivisor(beatTimeDivisor), OffsetRange(offsetRange)
{
}

FQTEDescription::~FQTEDescription()
{
}

void FQTEDescription::SetAttack(Attacks attack) {
	Attack = attack;
}