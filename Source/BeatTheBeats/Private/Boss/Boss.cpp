// Fill out your copyright notice in the Description page of Project Settings.


#include "Boss/Boss.h"
#include "Components\CapsuleComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Animation/AnimMontage.h"

ABoss::ABoss() : Super()
{
	PrimaryActorTick.bCanEverTick = true;
	BossState = EBossState::EBS_Chasing;
}

void ABoss::BeginPlay()
{
	Super::BeginPlay();

	Player = UGameplayStatics::GetPlayerPawn(this, 0);
}

void ABoss::OnBeat(float CurrentTimeSinceLastBeat)
{

	UE_LOG(LogTemp, Warning, TEXT("Attack On Beat!!"));
	if (bCanAttack)
	{
		UE_LOG(LogTemp, Warning, TEXT("Attack On Beat!!"));
		Attack();
	}
}

void ABoss::Attack()
{
	if (BossState == EBossState::EBS_Attacking) return;
	PlayAttackMontage();

	BossState = EBossState::EBS_Attacking;
}

void ABoss::PlayAttackMontage()
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && AttackMontage)
	{

		AnimInstance->Montage_Play(AttackMontage);
		AnimInstance->Montage_JumpToSection(FName("Attack3"), AttackMontage);
		UE_LOG(LogTemp, Warning, TEXT("Montage is Playing!!!"));
	}
}

void ABoss::DoDamage()
{

}

void ABoss::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	FVector PlayerLocation = Player->GetActorLocation();
	FVector BossLocation = GetActorLocation();

	float Distance = FVector::Dist(PlayerLocation, BossLocation);
	//UE_LOG(LogTemp, Warning, TEXT("Distance from Player to Boss: %f"), Distance);
	if (Distance <= AttackRange)
	{
		bCanAttack = true;
	} else 
	{
		bCanAttack = false;
	}
}

void ABoss::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

