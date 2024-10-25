// Fill out your copyright notice in the Description page of Project Settings.


#include "Boss/Boss.h"
#include "Components\CapsuleComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Animation/AnimMontage.h"
#include "AIController.h"

ABoss::ABoss() : Super()
{
	PrimaryActorTick.bCanEverTick = true;
	BossState = EBossState::EBS_StartChasing;

	Attack3EffectPos = CreateDefaultSubobject<USceneComponent>("EffectPos(Attack3)");
	Attack3EffectPos->SetupAttachment(GetRootComponent());
}

void ABoss::BeginPlay()
{
	Super::BeginPlay();

	Player = UGameplayStatics::GetPlayerPawn(this, 0);
	AIController = this->GetController<AAIController>();
	AIController->MoveToActor(Player);
}

void ABoss::OnBeat(float CurrentTimeSinceLastBeat)
{
	switch (BossState)
	{
	case EBossState::EBS_SlamAttacking:
		SlamAttack();
		break;
	case EBossState::EBS_BulletAttacking:
		break;
	case EBossState::EBS_RayAttacking:
		break;
	}
}

void ABoss::SlamAttack()
{
	if (!bCanAttack) return;
	RotateBoss = false;
	AIController->StopMovement();
	PlayAttackMontage();

	bCanAttack = false;
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

void ABoss::SpawnAttackParticleEffect(FName SocketName)
{
	RotateBoss = true;
	if (AttackParticleEffect && Attack3EffectPos)
	{
		UWorld* World = GetWorld();
		// Spawn the particle effect attached to the given scene component
		UGameplayStatics::SpawnEmitterAtLocation(World, AttackParticleEffect, Attack3EffectPos->GetComponentTransform(), true);
	}
}

void ABoss::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	FVector PlayerLocation = Player->GetActorLocation();
	FVector BossLocation = GetActorLocation();

	if (RotateBoss)
	{
		FVector Direction = (PlayerLocation - BossLocation).GetSafeNormal();
		FRotator TargetRotation = Direction.Rotation();
		float RotationSpeed = 2.f;

		FRotator CurrentRotation = GetActorRotation();
		FRotator NewRotation = FMath::RInterpTo(CurrentRotation, TargetRotation, GetWorld()->GetDeltaSeconds(), RotationSpeed); // RotationSpeed is a float that controls the speed
		SetActorRotation(FRotator(0, NewRotation.Yaw, 0));
	}
	float Distance = FVector::Dist(PlayerLocation, BossLocation);

	switch (BossState)
	{
	case EBossState::EBS_Unoccupied:
		RotateBoss = false;
		BossState = EBossState::EBS_StartChasing;
		break;
	case EBossState::EBS_Chasing:
		if (Distance <= AttackRange)
		{
			bCanAttack = true;
		}
		else {
			bCanAttack = false;
		}

		if (bCanAttack) 
			BossState = EBossState::EBS_SlamAttacking;
		break;
	case EBossState::EBS_StartChasing:
		AIController->MoveToActor(Player);
		BossState = EBossState::EBS_Chasing;
		break;
	default:
		break;
	}


}

void ABoss::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

