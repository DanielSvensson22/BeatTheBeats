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

UCLASS()
class BEATTHEBEATS_API ABoss : public AEnemyBase
{
	GENERATED_BODY()

public:
	ABoss();
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

protected:
	virtual void BeginPlay() override;

	virtual void OnBeat(float CurrentTimeSinceLastBeat) override;

	void SlamAttack();

	void RayAttack();

	void StartRayAttack();

	void PlayMontage(UAnimMontage* Montage, FName Section);

	virtual void DoDamage() override;

	void SpawnAttackParticleEffect(UParticleSystem* Particle, const FTransform& SpawnTransform);


	UFUNCTION(BlueprintCallable, Category = "Effects")
	void ParticleEffects();

public:

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

protected:
	UPROPERTY(BlueprintReadWrite)
	EBossState BossState;

private:
	UPROPERTY(EditDefaultsOnly, Category = "Montages")
	TObjectPtr<UAnimMontage> AttackMontage;

	UPROPERTY(EditDefaultsOnly, Category = "Montages")
	TObjectPtr<UAnimMontage> MiscMontage;

	APawn* Player;

	UPROPERTY(VisibleAnywhere)
	USceneComponent* Attack3EffectPos;

	AAIController* AIController;

	bool RotateBoss = false;

	bool IsFalling;

	int BeatCounter = 0;

public:
	UFUNCTION(BlueprintCallable)
	FORCEINLINE bool IsSpawning() const { return BossState == EBossState::EBS_Falling; };

};
