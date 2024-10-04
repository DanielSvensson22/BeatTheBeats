// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "Combos/Combo.h"
#include <tuple>
#include "PlayerCameraType.h"
#include "PlayerCharacter.generated.h"

class UInputAction;
class USpringArmComponent;
class UCameraComponent;
class UComboManagerComponent;
class AEnemyBase;
class ABeatManager;
class AWeaponBase;
class UAnimMontage;

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

private:
	void AttackCallback(Attacks AttackType, float MotionValue, float AnimLength, int Combo, int ComboStep);
	void PlayAttackMontage(UAnimInstance* AnimInstance, FName SectionName);
	void SetTargetLockCamera();

	void OnBeat(float CurrentTimeSinceLastBeat);

	void ProcessIncomingAttacks();

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

	Attacks CurrentBlockedType;

	bool bIsBlocking = false;

	/**
	* Animation Montages
	*/
	UPROPERTY(EditDefaultsOnly, Category = "Montages")
	TObjectPtr<UAnimMontage> AttackMontage;

public:
	FORCEINLINE ECameraState GetCameraState() const { return CameraState; }
	FORCEINLINE USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	FORCEINLINE bool GetIsLockingTarget() const { return bIsLockingTarget; }
};
