// Fill out your copyright notice in the Description page of Project Settings.


#include "Boss/Boss.h"
#include "Components\CapsuleComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Animation/AnimMontage.h"
#include "Components/SkeletalMeshComponent.h"
#include "AIController.h"

ABoss::ABoss() : Super()
{
	PrimaryActorTick.bCanEverTick = true;
	BossState = EBossState::EBS_Falling;

	Attack3EffectPos = CreateDefaultSubobject<USceneComponent>("EffectPos(Attack3)");
	Attack3EffectPos->SetupAttachment(GetRootComponent());
}

void ABoss::BeginPlay()
{
	Super::BeginPlay();

	Player = UGameplayStatics::GetPlayerPawn(this, 0);
	AIController = this->GetController<AAIController>();
	AIController->MoveToActor(Player);

	IsFalling = true;
}

void ABoss::OnBeat(float CurrentTimeSinceLastBeat)
{
	BeatCounter++;
	if (BeatCounter > 7) BeatCounter = 1;
	switch (BossState)
	{
	case EBossState::EBS_SlamAttacking:
		SlamAttack();
		break;
	case EBossState::EBS_BulletAttacking:
		break;
	case EBossState::EBS_StartRayAttacking:
		StartRayAttack();
		break;
	case EBossState::EBS_RayAttacking:
		RayAttack();
		break;
	}
}

void ABoss::SlamAttack()
{
	if (!bCanAttack) return;
	RotateBoss = false;
	AIController->StopMovement();
	PlayMontage(AttackMontage, "Attack3");

	bCanAttack = false;
}

void ABoss::RayAttack()
{
	if (!bCanAttack) return;
	PlayMontage(AttackMontage, "AttackRanged");
	bCanAttack = false;
}

void ABoss::StartRayAttack()
{
	if (!bCanAttack) return;
	AIController->StopMovement();
	ParticleEffects();
	BossState = EBossState::EBS_RayAttacking;
}

void ABoss::PlayMontage(UAnimMontage* Montage, FName Section)
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && Montage)
	{
		AnimInstance->Montage_Play(Montage);
		AnimInstance->Montage_JumpToSection(Section, Montage);
	}
}



void ABoss::DoDamage()
{

}

void ABoss::SpawnAttackParticleEffect(UParticleSystem* Particle, const FTransform& SpawnTransform)
{
	if (Particle)
	{
		UWorld* World = GetWorld();
		// Spawn the particle effect attached to the given scene component
		UGameplayStatics::SpawnEmitterAtLocation(World, Particle, SpawnTransform, true);
	}
}

void ABoss::ParticleEffects()
{
	switch (BossState)
	{
	case EBossState::EBS_SlamAttacking:
		SpawnAttackParticleEffect(AttackSlam, Attack3EffectPos->GetComponentTransform());
		break;
	case EBossState::EBS_RayAttacking:
		if (USkeletalMeshComponent* SkeletalMeshComponent = Cast<USkeletalMeshComponent>(GetMesh()))
		{
			FTransform Transform = SkeletalMeshComponent->GetSocketTransform(FName("Muzzle_01")); // get the position where to start the Ray
			FVector EffectLocation = Transform.GetLocation();
			FVector PlayerLocation = Player->GetActorLocation();
			FVector Direction = (PlayerLocation - EffectLocation).GetSafeNormal();

#pragma region Ray Effect
			FRotator EffectRotation = FRotationMatrix::MakeFromX(Direction).Rotator();
			Transform.SetRotation(EffectRotation.Quaternion());
			SpawnAttackParticleEffect(PrimaryAttackRay, Transform);
			SpawnAttackParticleEffect(PrimaryAttackMuzzle, Transform);
#pragma endregion


			FVector Start = EffectLocation;
			FVector End = Start + (Direction * 2000);
			FCollisionQueryParams CollisionParams;
			CollisionParams.AddIgnoredActor(this);
			FHitResult HitResult;
			bool bHit = GetWorld()->LineTraceSingleByChannel(
				HitResult,
				Start,
				End,
				ECC_Visibility,
				CollisionParams
			);

			if (bHit)
			{

				FVector HitLocation = HitResult.Location;
				FRotator HitRotation = HitResult.ImpactNormal.Rotation();
				FTransform HitTransform;
				HitTransform.SetLocation(HitLocation);
				HitTransform.SetRotation(HitRotation.Quaternion());
				HitTransform.SetScale3D(FVector(1.0f));
				SpawnAttackParticleEffect(PrimaryAttackGroundImpact, HitTransform);
			}

		}
		break;
	case EBossState::EBS_StartRayAttacking:
		if (USkeletalMeshComponent* SkeletalMeshComponent = Cast<USkeletalMeshComponent>(GetMesh()))
		{
			FTransform Transform = SkeletalMeshComponent->GetSocketTransform(FName("MuzzleLeft"));
			SpawnAttackParticleEffect(PrimaryAttackMuzzle, Transform);
			Transform = SkeletalMeshComponent->GetSocketTransform(FName("MuzzleRight"));
			SpawnAttackParticleEffect(PrimaryAttackMuzzle, Transform);
		}
		break;
	}
}

void ABoss::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	FVector PlayerLocation = Player->GetActorLocation();
	FVector BossLocation = GetActorLocation();
	float Distance = FVector::Dist(PlayerLocation, BossLocation);
	if (BossState == EBossState::EBS_Falling)
	{
		UE_LOG(LogTemp, Warning, TEXT("FALLING!!"));
		FVector Start = GetActorLocation();
		FVector End = Start + (FVector(0, 0, -1) * 400);
		FCollisionQueryParams CollisionParams;
		CollisionParams.AddIgnoredActor(this);
		FHitResult HitResult;
		bool bHit = GetWorld()->LineTraceSingleByChannel(
			HitResult,
			Start,
			End,
			ECC_PhysicsBody,
			CollisionParams
		);
		if (bHit && IsFalling) {
			Distance = FVector::Dist(PlayerLocation, HitResult.Location);
			PlayMontage(MiscMontage, "Land");
			IsFalling = false;
		}

	}

	if (RotateBoss)
	{
		FVector Direction = (PlayerLocation - BossLocation).GetSafeNormal();
		FRotator TargetRotation = Direction.Rotation();
		float RotationSpeed = 2.f;

		FRotator CurrentRotation = GetActorRotation();
		FRotator NewRotation = FMath::RInterpTo(CurrentRotation, TargetRotation, GetWorld()->GetDeltaSeconds(), RotationSpeed); // RotationSpeed is a float that controls the speed
		SetActorRotation(FRotator(0, NewRotation.Yaw, 0));
	}

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
			BossState = EBossState::EBS_SlamAttacking;
		}
		else if (Distance > AttackRange * 3 && BeatCounter == 1)
		{
			bCanAttack = true;
			BossState = EBossState::EBS_StartRayAttacking;
		}

		break;
	case EBossState::EBS_StartChasing:
		AIController->MoveToActor(Player);
		BossState = EBossState::EBS_Chasing;
		break;
	case EBossState::EBS_RayAttacking:
		RotateBoss = true;
		break;
	default:
		break;
	}
}

void ABoss::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}
