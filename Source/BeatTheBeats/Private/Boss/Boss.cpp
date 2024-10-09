// Fill out your copyright notice in the Description page of Project Settings.


#include "Boss/Boss.h"
#include "Components\CapsuleComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Animation/AnimMontage.h"
#include "AIController.h"

ABoss::ABoss() : Super()
{
	PrimaryActorTick.bCanEverTick = true;
	BossState = EBossState::EBS_Chasing;

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
	if (bCanAttack)
	{
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

void ABoss::SpawnAttackParticleEffect(FName SocketName)
{
	RotateBoss = true;
	if (AttackParticleEffect && Attack3EffectPos)
	{
		UWorld* World = GetWorld();
		// Spawn the particle effect attached to the given scene component
		UGameplayStatics::SpawnEmitterAtLocation(World, AttackParticleEffect, Attack3EffectPos->GetComponentTransform(), true);

		//UGameplayStatics::SpawnEmitterAttached(AttackParticleEffect, Attack3EffectPos, NAME_None);
	}
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
	}
	else {
		bCanAttack = false;
	}

	if (RotateBoss)
	{
		FVector Direction = (PlayerLocation - BossLocation).GetSafeNormal();
		FRotator TargetRotation = Direction.Rotation();
		float RotationSpeed = 1.f;

		FRotator CurrentRotation = GetActorRotation();
		FRotator NewRotation = FMath::RInterpTo(CurrentRotation, TargetRotation, GetWorld()->GetDeltaSeconds(), RotationSpeed); // RotationSpeed is a float that controls the speed
		SetActorRotation(NewRotation);
	}

	if (BossState == EBossState::EBS_Unoccupied) 
	{
		RotateBoss = false;
	}
	else if (BossState == EBossState::EBS_Chasing)
	{
		BossState = EBossState::EBS_Chasing;
		AIController->MoveToActor(Player);
	}


}

void ABoss::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

