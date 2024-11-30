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
class UNiagaraComponent;
class UMaterialInstanceDynamic;
class UTextBlock;
class UAudioComponent;

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

	virtual void ApplyPushBack(float Force) override;

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

	UPROPERTY(VisibleAnywhere)
	UAudioComponent* AudioComponent;

	UPROPERTY(EditAnywhere, Category = "Sounds")
	USoundBase* HitSound; // No need forward declaring

	UPROPERTY(EditAnywhere, Category = "Sounds")
	USoundBase* PerfectHitSound;

	UPROPERTY(EditAnywhere, Category = "Sounds")
	USoundBase* DeathSound;

	/**
	* Particles
	*/
	UPROPERTY(EditAnywhere, Category = "VisualEffects")
	UParticleSystem* HitParticles; // No need forward declaring

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	UWidgetComponent* EnemyWidget;

protected:

	virtual void DoDamage();

	void SetEffectsColor(Attacks Type);

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

	//VFX

	UNiagaraComponent* AttackTypeEffectComp;

	UPROPERTY(EditDefaultsOnly, Category = "VFX")
	UNiagaraSystem* AttackTypeEffect;

	UPROPERTY(EditDefaultsOnly, Category = "VFX")
	float SpawnRate = 35;

	UPROPERTY(EditDefaultsOnly, Category = "VFX")
	FLinearColor NeutralColor;

	UPROPERTY(EditDefaultsOnly, Category = "VFX")
	FLinearColor AttackOneColor;

	UPROPERTY(EditDefaultsOnly, Category = "VFX")
	FLinearColor AttackTwoColor;

	UPROPERTY(EditDefaultsOnly, Category = "VFX")
	FLinearColor AttackThreeColor;

	//Material VFX

	UPROPERTY(EditDefaultsOnly, Category = "MaterialVFX")
	int AttackTypeMaterialIndex;

	UPROPERTY(EditDefaultsOnly, Category = "MaterialVFX")
	FLinearColor LowNeutralColor;

	UPROPERTY(EditDefaultsOnly, Category = "MaterialVFX")
	FLinearColor HighNeutralColor;

	UPROPERTY(EditDefaultsOnly, Category = "MaterialVFX")
	FLinearColor LowAttack1Color;

	UPROPERTY(EditDefaultsOnly, Category = "MaterialVFX")
	FLinearColor HighAttack1Color;

	UPROPERTY(EditDefaultsOnly, Category = "MaterialVFX")
	FLinearColor LowAttack2Color;

	UPROPERTY(EditDefaultsOnly, Category = "MaterialVFX")
	FLinearColor HighAttack2Color;

	UPROPERTY(EditDefaultsOnly, Category = "MaterialVFX")
	FLinearColor LowAttack3Color;

	UPROPERTY(EditDefaultsOnly, Category = "MaterialVFX")
	FLinearColor HighAttack3Color;

	UPROPERTY(EditDefaultsOnly, Category = "MaterialVFX")
	FName LowColorName;

	UPROPERTY(EditDefaultsOnly, Category = "MaterialVFX")
	FName HighColorName;

	UMaterialInstanceDynamic* AttackTypeMaterial;

	//Damage indicators

	TArray<UTextBlock*> DamageIndicators;
	TArray<UTextBlock*> UsedDamageIndicators;

	TArray<float> DamageTimers;

	TArray<FVector2D> StartPositions;
	TArray<FVector2D> UsedStartPositions;

	UPROPERTY(EditDefaultsOnly, Category = "Damage indicators")
	float DamageIndicatorLifetime = 0.5f;

	UPROPERTY(EditDefaultsOnly, Category = "Damage indicators")
	FVector2D DamageIndicatorVelocity;

	//Push back

	float PushbackSpeed;

	UPROPERTY(EditDefaultsOnly)
	float PushbackDuration = 0.5f;

	float PushbackTimeLeft = 0;
};
