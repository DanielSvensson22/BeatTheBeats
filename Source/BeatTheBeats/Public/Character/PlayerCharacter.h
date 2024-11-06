// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "Combos/Combo.h"
#include <tuple>
#include "PlayerCameraType.h"
#include "../Combos/QTEDescription.h"
#include "PlayerCharacter.generated.h"

class UInputAction;
class USpringArmComponent;
class UCameraComponent;
class UComboManagerComponent;
class AEnemyBase;
class ABeatManager;
class AWeaponBase;
class UAnimMontage;
class UQTEComponent;
class ULegacyCameraShake;
class UNiagaraSystem;
class AScoreManager;

UCLASS()
class BEATTHEBEATS_API APlayerCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	APlayerCharacter();

	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;

	UFUNCTION(BlueprintCallable)
	void SetWeaponCollisionEnabled(ECollisionEnabled::Type CollisionEnable);

	void PlayAttackMontage(UAnimMontage* montage, FName SectionName, bool AddTimeBetweenBeats);

	UFUNCTION(BlueprintPure)
	FORCEINLINE float GetPlayerHealth() const { return CurrentHealth; }

	UFUNCTION(BlueprintPure)
	FORCEINLINE float GetMaxPlayerHealth() const { return MaxHealth; }

	UFUNCTION(BlueprintPure)
	FORCEINLINE bool IsAlive() const { return CurrentHealth > 0; }

	FORCEINLINE void SetNotifyName(const FString& name) {
		AttackNotifyName = name;
	}

protected:
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UComboManagerComponent> ComboManager;

	/**
	* InputActions
	*/
	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<UInputAction> MovementAction;

	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<UInputAction> LookAction;

	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<UInputAction> TargetLockAction;

	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<UInputAction> NeautralAttackAction;

	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<UInputAction> Type1AttackAction;

	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<UInputAction> Type2AttackAction;

	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<UInputAction> Type3AttackAction;

	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<UInputAction> NeautralBlockAction;

	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<UInputAction> Type1BlockAction;

	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<UInputAction> Type2BlockAction;

	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<UInputAction> Type3BlockAction;

	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<UInputAction> CameraShakeAction;
	
	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<UInputAction> ParticleAction;

	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<UInputAction> DodgeBackAction;

	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<UInputAction> DodgeLeftAction;

	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<UInputAction> DodgeRightAction;
	
	//Effects
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effects")
	UNiagaraSystem* TempParticleEffect;

	/**
	* Callbacks for input
	*/
	void Move(const FInputActionValue& Value);
	void Look(const FInputActionValue& Value);
	void TargetLock();

	void AddNeutralAttack();
	void AddType1Attack();
	void AddType2Attack();
	void AddType3Attack();

	void AddNeutralBlock();
	void AddType1Block();
	void AddType2Block();
	void AddType3Block();

	void DodgeBack();
	void DodgeLeft();
	void DodgeRight();

	void CameraShake();
	void SpawnParticle();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Reload")
	void ReloadLevel();

private:
	void AttackCallback(Attacks AttackType, float MotionValue, float AnimLength, int Combo, int ComboStep);
	void SetTargetLockCamera();

	void OnBeat(float CurrentTimeSinceLastBeat);

	void ProcessIncomingAttacks();

	void EnterQTE();
	void ExitQTE();

	void ApplyDamage(float Damage);

	void RotatePlayerToAttack(float DeltaTime);
	void MovePlayerToAttack(float DeltaTime);
	void PerformDodge(float DeltaTime);

private:
	typedef std::tuple<AEnemyBase*, Attacks, float> IncomingAttack;

	/**
	* Weapon Components
	*/
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<AWeaponBase> Weapon;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<AWeaponBase> WeaponClass;

	/**
	* Camera Components
	*/
	ECameraState CameraState = ECameraState::ECS_FreeCamera;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USpringArmComponent> CameraBoom;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UCameraComponent> ViewCamera;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UCameraShakeBase> CameraShakeClass;

	/**
	* Target Lock Components
	*/
	UPROPERTY(VisibleAnywhere, Category = "Target Lock")
	TObjectPtr<AActor> TargetLockHitTarget;

	UPROPERTY(VisibleAnywhere, Category = "Target Lock")
	bool bIsLockingTarget = false;

	UPROPERTY(EditAnywhere, Category = "Target Lock")
	float LockOffsetModifier;

	UPROPERTY(EditAnywhere, Category = "Target Lock")
	float TargetLockTraceRange;

	UPROPERTY(EditAnywhere, Category = "Target Lock")
	float TargetLockTraceRadius;

	UPROPERTY(EditAnywhere, Category = "Target Lock")
	float TargetLockMaxMoveDistance; // Max distance between target and player before auto disable

	/**
	* Combat Components
	*/
	TArray<IncomingAttack> IncomingAttacks;

	class ABeatManager* BeatManager;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<ABeatManager> BeatManagerClass;

	FDelegateHandle BeatHandle;

	Attacks CurrentBlockedType;

	bool bIsAttacking = false;

	UPROPERTY(EditDefaultsOnly)
	float ClosenessForPeffectBlock = 0.7f;

	bool bPerfectBlock = false;

	bool bIsBlocking = false;

	float CurrentHealth;

	UPROPERTY(EditDefaultsOnly)
	float MaxHealth = 100;

	bool bHasDied = false;

	UPROPERTY(EditDefaultsOnly)
	float PlayerDamage = 10;

	bool bMovedThisTick = false;

	UPROPERTY(EditDefaultsOnly)
	float RotationSpeed = 10;

	AScoreManager* ScoreManager;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<AScoreManager> ScoreManagerClass;

	UPROPERTY(VisibleAnywhere)
	UQTEComponent* QTE;

	UPROPERTY(EditDefaultsOnly)
	UNiagaraSystem* BlockEffect;

	bool bClosingDistance = false;

	UPROPERTY(EditDefaultsOnly)
	float ClosingDistanceSpeed = 1.0f;

	AEnemyBase* EnemyToAttack;

	UPROPERTY(EditDefaultsOnly)
	float MaxClosingDistance = 200;

	UPROPERTY(EditDefaultsOnly)
	float MinClosingDistance = 50;

	//Dodging

	bool bIsDodging = false;

	FVector DodgeLocation;

	UPROPERTY(EditDefaultsOnly)
	float DodgeDistance = 100;

	UPROPERTY(EditDefaultsOnly)
	float DodgeSpeed = 100;

	UPROPERTY(EditDefaultsOnly)
	float InvincibilityDuration = 0.2f;

	float TimeUntilInvincibilityEnds;

	//Montages

	UPROPERTY(EditDefaultsOnly, Category = "Montages")
	UAnimMontage* DeathAnim;

	UPROPERTY(EditDefaultsOnly, Category = "Montages")
	UAnimMontage* BlockAnim;

	UPROPERTY(EditDefaultsOnly, Category = "Montages")
	UAnimMontage* DodgeBackAnim;

	UPROPERTY(EditDefaultsOnly, Category = "Montages")
	UAnimMontage* DodgeLeftAnim;

	UPROPERTY(EditDefaultsOnly, Category = "Montages")
	UAnimMontage* DodgeRightAnim;

	//Debug
	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<UInputAction> QTEAction;

	UPROPERTY(EditDefaultsOnly)
	TArray<FQTEDescription> DebugQTEDescription;

	bool bInQTE = false;

	FString AttackNotifyName;

public:
	FORCEINLINE ECameraState GetCameraState() const { return CameraState; }
	FORCEINLINE USpringArmComponent* GetCameraBoom() const { return CameraBoom; }

	UFUNCTION(BlueprintPure)
	FORCEINLINE bool GetIsLockingTarget() const { return bIsLockingTarget; }
};
