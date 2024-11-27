// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Enemy/Enemy.h"
#include "Boss.generated.h"

UENUM(BlueprintType)
enum class EBossState : uint8
{
	EBS_Unoccupied UMETA(DisplayName = "Unoccupied"),
	EBS_Rotating UMETA(DisplayName = "Rotating"),
	EBS_Spawning UMETA(DisplayName = "Spawning"),
	EBS_Falling UMETA(DisplayName = "Falling"),
	EBS_Chasing UMETA(DisplayName = "Chasing"),
	EBS_StartChasing UMETA(DisplayName = "StartChasing"),
	EBS_SlamAttacking UMETA(DisplayName = "Attacking"),
	EBS_RayAttacking UMETA(DisplayName = "RayAttacking"),
	EBS_StartRayAttacking UMETA(DisplayName = "RayAttacking"),
	EBS_BulletAttacking UMETA(DisplayName = "BulletAttacking"),
};

class AAIController;
class UAnimMontage;
class USkeletalMeshComponent;
class ABullet;

UCLASS()
class BEATTHEBEATS_API ABoss : public AEnemyBase
{
	GENERATED_BODY()

public:
	ABoss();
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// Scene swapping function
	UFUNCTION(BlueprintCallable, Category = "Boss")
	void SwapSceneOnDeath();

	void ApplyBulletDamage();

protected:
	virtual void BeginPlay() override;

	virtual void OnBeat(float CurrentTimeSinceLastBeat) override;

	void SlamAttack();

	void SlamDamage();

	void RayAttack();

	void BUlletAttack();

	void StartRayAttack();

	void PlayMontage(UAnimMontage* Montage, FName Section);

	virtual void DoDamage() override;

	void SpawnBullet();

	void SpawnAttackParticleEffect(UParticleSystem* Particle, const FTransform& SpawnTransform);


	UFUNCTION(BlueprintCallable, Category = "Effects")
	void ParticleEffects();

	UFUNCTION(BlueprintCallable, Category = "Sounds")
	void DeathSound();

	bool CheckIfNeedsToRotate();

public:

	// The level to load on death (assignable in the editor)
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Death")
	TSoftObjectPtr<UWorld> LevelToLoadOnDeath;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effects")
	UParticleSystem* AttackSlam;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effects")
	UParticleSystem* PrimaryAttackMuzzle;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effects")
	UParticleSystem* PrimaryAttackRay;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effects")
	UParticleSystem* PrimaryAttackImpact;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effects")
	UParticleSystem* PrimaryAttackGroundImpact;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effects")
	UParticleSystem* FallingTrailEffect;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effects")
	UParticleSystem* FallingSpawnEffect;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effects")
	UParticleSystem* LandingEffect;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effects")
	UParticleSystem* FallingResidualEffect;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effects")
	UParticleSystem* BulletSpawnEffect;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat") 
	TSubclassOf<ABullet> BulletClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawn")
	bool SpawnFromSky;

protected:
	UPROPERTY(BlueprintReadWrite)
	EBossState BossState;

private:
	UPROPERTY(EditDefaultsOnly, Category = "Montages")
	TObjectPtr<UAnimMontage> AttackMontage;

	UPROPERTY(EditDefaultsOnly, Category = "Montages")
	TObjectPtr<UAnimMontage> MiscMontage;

	UPROPERTY(EditDefaultsOnly, Category = "Montages")
	TObjectPtr<UAnimMontage> RotateMontage;

	UPROPERTY(EditDefaultsOnly, Category = "Damage")
	float SlamDamageMin;

	UPROPERTY(EditDefaultsOnly, Category = "Damage")
	float SlamDamageMax;

	UPROPERTY(EditDefaultsOnly, Category = "Damage")
	float RayDamage;

	UPROPERTY(EditDefaultsOnly, Category = "Damage")
	float BulletDamage;

	UPROPERTY(EditDefaultsOnly, Category = "Sounds")
	USoundBase* DeathSoundCue;

	UPROPERTY(EditDefaultsOnly, Category = "Sounds")
	USoundBase* SlamSoundCue;

	UPROPERTY(EditDefaultsOnly, Category = "Sounds")
	USoundBase* SlamSoundHitCue;

	UPROPERTY(EditDefaultsOnly, Category = "Sounds")
	USoundBase* RaySoundCue;

	UPROPERTY(EditDefaultsOnly, Category = "Sounds")
	USoundBase* RayStartSoundCue;


	APawn* Player;

	UPROPERTY(VisibleAnywhere)
	USceneComponent* Attack3EffectPos;

	UPROPERTY(VisibleAnywhere)
	USceneComponent* SlamImpact;

	AAIController* AIController;

	bool PlayingDeathSound = false;

	bool RotateBoss = false;

	bool IsFalling;

	int BeatCounter = 0;

	bool bHasStartedRotationMontage;

	float RotationDuration = 0.0f;  // Tracks how long boss has been rotating
	const float MaxRotationTime = 2.0f;  // Maximum time to rotate before snapping
	float RotationThreshold = 2.0f; // How much the boss needs to Rotate to count as facing towards the player

public:
	UFUNCTION(BlueprintCallable)
	FORCEINLINE bool IsSpawning() const { return BossState == EBossState::EBS_Falling; };


};
