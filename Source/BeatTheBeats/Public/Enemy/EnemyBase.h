// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "../Combos/Combo.h"
#include "Interfaces/HitInterface.h"
#include "Interfaces/LockOnInterface.h"
#include "EnemyBase.generated.h"

class ABeatManager;
class AEnemyQueue;
class UWidgetComponent;
class AScoreManager;
class UNiagaraSystem;

UCLASS()
class BEATTHEBEATS_API AEnemyBase : public ACharacter, public IHitInterface, public ILockOnInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AEnemyBase();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void OnBeat(float CurrentTimeSinceLastBeat);

	virtual void Attack();

	/**
	* Play Montage functions
	*/
	void PlayHitReactMontage(const FName& SectionName);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "DeathEvent")
	void Death();
	
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "HitEvent")
	void Hit();
	
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "PerfectHitEvent")
	void PerfectHit();

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual void GetHit(const FVector& ImpactPoint) override;
	void DirectionalHitReact(const FVector& ImpactPoint);

	virtual void ApplyDamage(float Damage, Attacks AttackType, bool OnBeat, FVector HitLocation) override;

	virtual bool GetCanAttack();

	FORCEINLINE float GetMaxAttackDistance() { return MaxAttackDistance; }
	FORCEINLINE bool GetCanChasePlayer() { return bCanChasePlayer; }

	UFUNCTION(BlueprintPure)
	FORCEINLINE bool IsAlive() { return CurrentHealth > 0; }

	FORCEINLINE Attacks GetEnemyType() { return EnemyType; }

	void SetAttackState(bool CanAttack, bool CanChase);

	void EnterQueue();

	void ExitQueue();

	virtual void Parry();

	UFUNCTION(BlueprintPure)
	FORCEINLINE float GetHealthPercent() const { return CurrentHealth / MaxHealth; }

	UFUNCTION(BlueprintPure)
	FORCEINLINE float GetPerformAttackPercent() const { return (float)(CurrentAttack + 1) / StandardCombo.AttackCount(); }

	FORCEINLINE bool GetIsStunned() { return bIsStunned; }

	FORCEINLINE float GetMaxViewDistance() { return MaxViewDistance; }

protected:

	class ABeatManager* BeatManager;

	UPROPERTY(EditDefaultsOnly)
	FCombo StandardCombo;

	UPROPERTY(EditDefaultsOnly)
	Attacks EnemyType;

	int CurrentAttack;

	bool bCanAttack = false;

	UPROPERTY(EditAnywhere)
	float MaxHealth = 100;

	UPROPERTY(VisibleAnywhere)
	float CurrentHealth = 100;

	bool bHasDied = false;

	UPROPERTY(EditDefaultsOnly)
	float AttackRange = 100;

	UPROPERTY(EditDefaultsOnly)
	float Damage = 10;

	UPROPERTY(VisibleAnywhere)
	bool bIsMelee = true;

	UPROPERTY(VisibleAnywhere)
	bool bIsStunned = false;

	/**
	* Animation Montages
	*/
	UPROPERTY(EditDefaultsOnly, Category = "Montages")
	UAnimMontage* HitReactMontage;

	/**
	* Sounds
	*/
	UPROPERTY(EditAnywhere, Category = "Sounds")
	USoundBase* HitSound; // No need forward declaring

	/**
	* Particles
	*/
	UPROPERTY(EditAnywhere, Category = "VisualEffects")
	UParticleSystem* HitParticles; // No need forward declaring

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	UWidgetComponent* EnemyWidget;

protected:

	virtual void DoDamage();

private:

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<ABeatManager> BeatManagerClass;

	FDelegateHandle BeatHandle;

	APawn* Player;

	UPROPERTY(EditDefaultsOnly)
	float MaxAttackDistance = 100;

	UPROPERTY(EditDefaultsOnly)
	float MaxViewDistance = 2000;

	bool bCanChasePlayer = false;

	bool bHasAddedToQueue = false;

	AEnemyQueue* EnemyQueue;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<AEnemyQueue> EnemyQueueClass;

	UPROPERTY(EditDefaultsOnly)
	float OptimalAttackMultiplier = 2;

	AScoreManager* ScoreManager;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<AScoreManager> ScoreManagerClass;

	UPROPERTY(EditDefaultsOnly)
	UNiagaraSystem* GetHitEffect;
};
