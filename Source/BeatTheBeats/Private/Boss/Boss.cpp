// Fill out your copyright notice in the Description page of Project Settings.
#include "Boss/Boss.h"
#include "Components/SphereComponent.h"
#include "Boss/Bullet.h"
#include "Components\CapsuleComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Animation/AnimMontage.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "AIController.h"
#include "Score/ScoreManager.h"
#include "Components/AudioComponent.h"
#include "Beats/BeatManager.h"

ABoss::ABoss() : Super()
{
	PrimaryActorTick.bCanEverTick = true;


	Attack3EffectPos = CreateDefaultSubobject<USceneComponent>("EffectPos(Attack3)");
	Attack3EffectPos->SetupAttachment(GetRootComponent());

	SlamImpact = CreateDefaultSubobject<USceneComponent>("SlamImpactPos");
	SlamImpact->SetupAttachment(GetRootComponent());

}

void ABoss::BeginPlay()
{
	EnemyType = Attacks::Attack_Neutral;
	Super::BeginPlay();

	Player = UGameplayStatics::GetPlayerPawn(this, 0);
	AIController = this->GetController<AAIController>();
	AIController->MoveToActor(Player);

	if (SpawnFromSky)
	{
		BossState = EBossState::EBS_Spawning;
	}
	else
	{
		BossState = EBossState::EBS_Unoccupied;
	}

	IsFalling = true;
}

void ABoss::OnBeat(float CurrentTimeSinceLastBeat)
{
	CurrentAttack = StandardCombo.NextAttack();

	if (bCanAttack) {
		if (CurrentAttack == StandardCombo.AttackCount() - 1) {
			switch (BossState)
			{
			case EBossState::EBS_SlamAttacking:
				SlamAttack();
				break;
			case EBossState::EBS_BulletAttacking:
				BUlletAttack();
				break;

			case EBossState::EBS_RayAttacking:
				RayAttack();
				break;
			}
		}

		if (CurrentAttack == StandardCombo.AttackCount() - 2 && BossState == EBossState::EBS_StartRayAttacking) {
			StartRayAttack();
		}
	}	
}

void ABoss::SlamAttack()
{
	if (!bCanAttack) return;
	RotateBoss = false;
	AIController->StopMovement();
	if (SlamSoundCue)  // Check if the sound cue is set
	{
		// Play the death sound at the boss's location
		//UGameplayStatics::PlaySoundAtLocation(this, SlamSoundCue, GetActorLocation());
		CheckVolume();
		AudioComponent->SetSound(SlamSoundCue);
		AudioComponent->Play();
	}

	PlayEnemyMontage(AttackMontage, "Attack3", BeatManager->TimeBetweenBeats() + BeatManager->TimeBetweenBeats() * 1.5f);

	bCanAttack = false;
}

void ABoss::SlamDamage()
{
	// Define impact point and radius
	FVector ImpactPoint = SlamImpact->GetComponentLocation();  // Adjust based on where the impact should be
	float ImpactRadius = 500.0f;  // The maximum radius within which damage applies

	// Calculate distance between player and impact point
	float DistanceToPlayer = FVector::Dist(ImpactPoint, Player->GetActorLocation());

	// Check if player is within impact radius
	if (DistanceToPlayer <= ImpactRadius)
	{
		// Calculate damage based on distance (linear falloff)
		float DamageFalloff = FMath::Clamp(1.0f - (DistanceToPlayer / ImpactRadius), 0.0f, 1.0f);
		float DamageAmount = FMath::Lerp(SlamDamageMin, SlamDamageMax, DamageFalloff);

		// Apply damage to player
		UGameplayStatics::ApplyDamage(Player, DamageAmount, GetController(), this, UDamageType::StaticClass());

		UE_LOG(LogTemp, Warning, TEXT("Player damaged by slam attack! Damage: %f"), DamageAmount);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Player is out of range of slam attack."));
	}
}

void ABoss::RayAttack()
{
	if (!bCanAttack) return;
	if (RaySoundCue)  // Check if the sound cue is set
	{
		// Play the death sound at the boss's location
		//UGameplayStatics::PlaySoundAtLocation(this, RaySoundCue, GetActorLocation());
		CheckVolume();
		AudioComponent->SetSound(RaySoundCue);
		AudioComponent->Play();
	}
	PlayEnemyMontage(AttackMontage, "AttackRanged", BeatManager->TimeBetweenBeats() * 2.95f);
	bCanAttack = false;
}

void ABoss::BUlletAttack()
{
	if (!bCanAttack) return;

	PlayEnemyMontage(AttackMontage, "Attack1", BeatManager->TimeBetweenBeats() + BeatManager->TimeBetweenBeats() * 1.5f);

	AIController->StopMovement();
	ParticleEffects();
	bCanAttack = false;
}

void ABoss::SpawnBullet()
{
	if (!BulletClass || !Player) return;

	// Spawn location and rotation
	FVector SpawnLocation = GetMesh()->GetSocketLocation("MuzzleCenter");  // Assuming a socket on the boss's hand
	FRotator SpawnRotation = (Player->GetActorLocation() - SpawnLocation).Rotation();

	if (BulletShootCue)
		//UGameplayStatics::PlaySoundAtLocation(this, BulletShootCue, GetActorLocation());
		CheckVolume();
		AudioComponent->SetSound(BulletShootCue);
		AudioComponent->Play();

	// Spawn the bullet
	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	ABullet* SpawnedBullet = GetWorld()->SpawnActor<ABullet>(BulletClass, SpawnLocation, SpawnRotation, SpawnParams);

	if (SpawnedBullet)
	{
		// Set velocity towards the player
		FVector Direction = (Player->GetActorLocation() - SpawnLocation).GetSafeNormal();
		SpawnedBullet->ProjectileMovement->Velocity = Direction * SpawnedBullet->ProjectileMovement->InitialSpeed;
		SpawnedBullet->Boss = this;
		SpawnedBullet->Player = Player;
	}
}

void ABoss::SwapSceneOnDeath()
{
	if (LevelToLoadOnDeath.IsNull())
	{
		UE_LOG(LogTemp, Warning, TEXT("LevelToLoad is not set!"));
		return;
	}

	// Resolve the soft reference and convert it to FName
	FName LevelName(*LevelToLoadOnDeath.GetLongPackageName());
	if (!LevelName.IsNone())
	{
		UE_LOG(LogTemp, Warning, TEXT("Loading Level: %s"), *LevelName.ToString());
		UGameplayStatics::OpenLevel(this, LevelName);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Invalid LevelToLoad!"));
	}
}

void ABoss::ApplyBulletDamage()
{
	UGameplayStatics::ApplyDamage(Player, BulletDamage, GetController(), this, UDamageType::StaticClass());
}

void ABoss::SaveScoreOnDeath()
{
	AScoreManager* score = Cast<AScoreManager>(UGameplayStatics::GetActorOfClass(this, ScoreClass));

	if (score) {
		score->Save();
	}
}

void ABoss::StartRayAttack()
{
	if (!bCanAttack) return;

	if (RayStartSoundCue)  // Check if the sound cue is set
	{
		// Play the death sound at the boss's location
		//UGameplayStatics::PlaySoundAtLocation(this, RayStartSoundCue, GetActorLocation());
		CheckVolume();
		AudioComponent->SetSound(RayStartSoundCue);
		AudioComponent->Play();
	}

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
	case EBossState::EBS_BulletAttacking:
		if (USkeletalMeshComponent* SkeletalMeshComponent = Cast<USkeletalMeshComponent>(GetMesh()))
		{
			if (bCanAttack)
			{
				UGameplayStatics::SpawnEmitterAttached(
					BulletSpawnEffect,         // The particle system to spawn 
					GetMesh(),              // The mesh to attach to 
					FName("MuzzleCenter"),             // The socket name 
					FVector::ZeroVector,    // Location offset (relative to socket)
					FRotator::ZeroRotator,  // Rotation offset (relative to socket) 
					EAttachLocation::SnapToTarget, // Snap to the socket's transform 
					true                    // Auto-destroy the particle system 
				);
			}
			else
			{
				FTransform Transform = SkeletalMeshComponent->GetSocketTransform(FName("MuzzleCenter"));
				SpawnAttackParticleEffect(PrimaryAttackMuzzle, Transform);

				SpawnBullet();
			}
		}
		break;
	case EBossState::EBS_SlamAttacking:
		SpawnAttackParticleEffect(AttackSlam, Attack3EffectPos->GetComponentTransform());
		SlamDamage();
		if (SlamSoundHitCue)  // Check if the sound cue is set
		{
			// Play the death sound at the boss's location
			//UGameplayStatics::PlaySoundAtLocation(this, SlamSoundHitCue, GetActorLocation());
			CheckVolume();
			AudioComponent->SetSound(SlamSoundHitCue);
			AudioComponent->Play();
		}
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

				if (HitResult.GetActor() && HitResult.GetActor() == Player)
				{
					// Apply damage to player
					UGameplayStatics::ApplyDamage(Player, RayDamage, GetController(), this, UDamageType::StaticClass());
				}

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

void ABoss::DeathSound()
{
	if (PlayingDeathSound) return;
	PlayingDeathSound = true;

	if (DeathSoundCue)  // Check if the sound cue is set
	{
		// Play the death sound at the boss's location
		//UGameplayStatics::PlaySoundAtLocation(this, DeathSoundCue, GetActorLocation());
		CheckVolume();
		AudioComponent->SetSound(DeathSoundCue);
		AudioComponent->Play();
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("DeathSoundCue is not set!"));
	}

}

bool ABoss::CheckIfNeedsToRotate()
{
	if (!Player) return false;  // Ensure you have a reference to the player

	// Get the boss and player locations and project them onto the 2D plane
	FVector BossLocation = GetActorLocation();
	BossLocation.Z = 0;

	FVector PlayerLocation = Player->GetActorLocation();
	PlayerLocation.Z = 0;

	// Get the 2D direction vector to the player
	FVector DirectionToPlayer = (PlayerLocation - BossLocation).GetSafeNormal();

	// Get the forward vector of the boss and set Z to 0
	FVector BossForward = GetActorForwardVector();
	BossForward.Z = 0;
	BossForward.Normalize();

	// Calculate the angle between the forward vector and the direction to the player
	float AngleDegrees = FMath::Acos(FVector::DotProduct(BossForward, DirectionToPlayer)) * (180.0f / PI);

	// Define a threshold, e.g., 20 degrees

	// Check if the angle exceeds the threshold
	return AngleDegrees > RotationThreshold;
}

void ABoss::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	APawn* p = UGameplayStatics::GetPlayerPawn(this, 0);

	if (p == nullptr) {
		return;
	}

	FVector PlayerLocation = Player->GetActorLocation();
	FVector BossLocation = GetActorLocation();
	float Distance = FVector::Dist(PlayerLocation, BossLocation);
	if (BossState == EBossState::EBS_Falling)
	{
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
			SpawnAttackParticleEffect(AttackSlam, Attack3EffectPos->GetComponentTransform());
			IsFalling = false;
		}

	}


	FVector Start = GetActorLocation();
	FVector End = Player->GetActorLocation();
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

	bool CanShoot = false;
	if (HitResult.GetActor() && HitResult.GetActor() == Player) CanShoot = true;

	if (BossState == EBossState::EBS_Rotating)
	{
		if (!CheckIfNeedsToRotate())
		{
			BossState = EBossState::EBS_Unoccupied;
			bHasStartedRotationMontage = false;  // Reset flag when done rotating
			RotationDuration = 0.0f;  // Reset rotation duration when done
			return;
		}

		// Calculate direction and cross product for animation montage selection
		FVector Direction = (PlayerLocation - BossLocation).GetSafeNormal();
		FVector BossForward = GetActorForwardVector();
		FVector CrossProduct = FVector::CrossProduct(BossForward, Direction);

		// Perform the smooth rotation with dynamic speed
		float AngleDegrees = FMath::Acos(FVector::DotProduct(BossForward, Direction)) * (180.0f / PI);
		float RotationSpeed = FMath::GetMappedRangeValueClamped(FVector2D(0, 180), FVector2D(1, 4), AngleDegrees);

		// Start the rotation montage if it hasn't started yet
		if (!bHasStartedRotationMontage)
		{
			if (CrossProduct.Z > 0) // Left Turn
			{
				if (AngleDegrees > 15 && AngleDegrees < 90)
				{
					PlayMontage(RotateMontage, "TurnShortRight");
				}
				else if (AngleDegrees >= 90)
				{
					PlayMontage(RotateMontage, "TurnLongRight");
				}
			}
			else if (CrossProduct.Z < -0) // Right Turn
			{
				if (AngleDegrees > 15 && AngleDegrees < 90)
				{
					PlayMontage(RotateMontage, "TurnShortLeft");
				}
				else if (AngleDegrees >= 90)
				{
					PlayMontage(RotateMontage, "TurnLongLeft");
				}
			}

			bHasStartedRotationMontage = true;  // Set the flag to avoid re-triggering the montage
		}

		// Interpolate the rotation and set the new rotation
		FRotator TargetRotation = Direction.Rotation();
		FRotator CurrentRotation = GetActorRotation();
		FRotator NewRotation = FMath::RInterpTo(CurrentRotation, TargetRotation, GetWorld()->GetDeltaSeconds(), RotationSpeed * (1 + RotationDuration * 1.0f));
		SetActorRotation(FRotator(0, NewRotation.Yaw, 0));

		// Increment rotation duration
		RotationDuration += GetWorld()->GetDeltaSeconds();

		// Snap rotation if it exceeds maximum rotation time
		if (RotationDuration >= MaxRotationTime)
		{
			SetActorRotation(FRotator(0, TargetRotation.Yaw, 0));  // Snap to target
			RotationDuration = 0.0f;  // Reset the duration counter
			BossState = EBossState::EBS_Unoccupied;  // Set to unoccupied or another state
			bHasStartedRotationMontage = false;  // Reset montage flag if done
		}
	}


	switch (BossState)
	{
	case EBossState::EBS_Spawning:

		UGameplayStatics::SpawnEmitterAttached(
			FallingTrailEffect,
			GetMesh(),
			FName("Death_Cod"),
			FVector::ZeroVector,
			FRotator::ZeroRotator,
			EAttachLocation::SnapToTarget,
			true
		);

		if (USkeletalMeshComponent* SkeletalMeshComponent = Cast<USkeletalMeshComponent>(GetMesh()))
		{
			FTransform Transform = SkeletalMeshComponent->GetSocketTransform("Death_Cod");
			SpawnAttackParticleEffect(FallingSpawnEffect, Transform);
			Transform = SkeletalMeshComponent->GetSocketTransform("FX_LandingImpact");
			SpawnAttackParticleEffect(LandingEffect, Transform);
			SpawnAttackParticleEffect(FallingResidualEffect, Transform);
		}

		BossState = EBossState::EBS_Falling;
		break;
	case EBossState::EBS_Unoccupied:
		if (CheckIfNeedsToRotate()) {
			BossState = EBossState::EBS_Rotating;
			bHasStartedRotationMontage = false;
		}
		else {
			BossState = EBossState::EBS_StartChasing;
		}

		break;
	case EBossState::EBS_Chasing:
		if (Distance <= AttackRange)
		{
			bCanAttack = true;
			BossState = EBossState::EBS_SlamAttacking;
		}
		else if (Distance > AttackRange * 1.5f && Distance < AttackRange * 3.5 && CurrentAttack == StandardCombo.AttackCount() - 3 && CanShoot)
		{
			bCanAttack = true;
			BossState = EBossState::EBS_BulletAttacking;
		}
		else if (Distance > AttackRange * 3.5 && CurrentAttack == StandardCombo.AttackCount() - 3 && CanShoot)
		{
			bCanAttack = true;
			BossState = EBossState::EBS_StartRayAttacking;
		}

		break;
	case EBossState::EBS_BulletAttacking:

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
