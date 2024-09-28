// Fill out your copyright notice in the Description page of Project Settings.


#include "Boss/Boss.h"
#include "Components\CapsuleComponent.h"

ABoss::ABoss() : Super()
{
	
}

void ABoss::BeginPlay()
{
	Super::BeginPlay();
	
}

void ABoss::OnBeat(float CurrentTimeSinceLastBeat)
{

}

void ABoss::Attack()
{

}

void ABoss::DoDamage()
{

}

void ABoss::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ABoss::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

