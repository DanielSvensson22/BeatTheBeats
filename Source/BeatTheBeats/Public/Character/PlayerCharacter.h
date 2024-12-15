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

class UTutorialChecklist;
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
class UNiagaraComponent;
class UMaterialInstanceDynamic;
class AScoreManager;
class UAudioComponent;

UCLASS()
class BEATTHEBEATS_API APlayerCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	APlayerCharacter();

	~APlayerCharacter();

	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;

	UFUNCTION(BlueprintCallable)
		void SetWeaponCollisionEnabled(ECollisionEnabled::Type CollisionEnable);

	void PlayAttackMontage(UAnimMontage* montage, FName SectionName, float TotalTime);

	UFUNCTION(BlueprintPure)
		FORCEINLINE float GetPlayerHealth() const { return CurrentHealth; }

	UFUNCTION(BlueprintPure)
		FORCEINLINE float GetMaxPlayerHealth() const { return MaxHealth; }

	UFUNCTION(BlueprintPure)
		FORCEINLINE bool IsAlive() const { return CurrentHealth > 0; }

	FORCEINLINE void SetNotifyName(const FString& name) {
		AttackNotifyName = name;
	}

	FORCEINLINE bool InAttackAnimation() { return bInAttackAnimation; }

	void EnterQTE();
	void ExitQTE();

	void FailedSpecial();
	void Special1();
	void Special2();
	void Special3();

	FORCEINLINE AWeaponBase* GetWeapon() { return Weapon; }

	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<UTutorialChecklist> Checklist;

	UFUNCTION(BlueprintPure)
	FORCEINLINE UTutorialChecklist* GetTutorialChecklist() const { return Checklist; }

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

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "OnDeath")
		void OnDeath();
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "OnHit")
		void OnHit();

private:
	void AttackCallback(TArray<FQTEDescription>* qte, ComboEffect effect);
	void SetTargetLockCamera();

	void OnBeat(float CurrentTimeSinceLastBeat);

	void ProcessIncomingAttacks();

	void ApplyDamage(float Damage, FVector HitLocation, FRotator HitRotation);

	void RotatePlayerToAttack(float DeltaTime);
	void MovePlayerToAttack(float DeltaTime);
	void PerformDodge(float DeltaTime);

private:
	typedef std::tuple<AEnemyBase*, Attacks, float, FVector> IncomingAttack;

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
	TArray<AActor*> HitArray;
	float ClosestDistance = 100000.f;

	UPROPERTY(VisibleAnywhere, Category = "Target Lock")
	TObjectPtr<AEnemyBase> EnemyToLock;

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

	UPROPERTY()
		class ABeatManager* BeatManager;

	UPROPERTY(EditDefaultsOnly)
		TSubclassOf<ABeatManager> BeatManagerClass;

	FDelegateHandle BeatHandle;

	Attacks CurrentBlockedType;

	bool bIsAttacking = false;

	bool bInAttackAnimation = false;

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

	TArray<FQTEDescription>* CurrentQTEDescription;
	ComboEffect CurrentComboEffect;

	//Specials

	UPROPERTY(EditDefaultsOnly, Category = "Specials")
		float FailedSpecialDamage = 30;

	UPROPERTY(EditDefaultsOnly, Category = "Specials")
		float Special1Damage = 60;

	UPROPERTY(EditDefaultsOnly, Category = "Specials")
		float Special2Damage = 50;

	UPROPERTY(EditDefaultsOnly, Category = "Specials")
		float Special3Damage = 50;

	UPROPERTY(EditDefaultsOnly, Category = "Specials")
		float Special2QTESpeedIncrease = 0.5f;

	UPROPERTY(EditDefaultsOnly, Category = "Specials")
		TArray<FQTEDescription> Special2QTE;

	bool bSpecial2Active = false;

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

	UPROPERTY(EditDefaultsOnly, Category = "Montages")
		UAnimMontage* StartAnim;

	UPROPERTY(EditDefaultsOnly, Category = "Montages")
		UAnimMontage* FailedSpecialAnim;

	UPROPERTY(EditDefaultsOnly, Category = "Montages")
		UAnimMontage* Special1Anim;

	UPROPERTY(EditDefaultsOnly, Category = "Montages")
		UAnimMontage* Special2Anim;

	UPROPERTY(EditDefaultsOnly, Category = "Montages")
		UAnimMontage* Special3Anim;

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

	UPROPERTY(EditDefaultsOnly, Category = "VFX")
		UNiagaraSystem* TookDamageEffect;

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

	//Sound

	UPROPERTY(VisibleAnywhere)
		UAudioComponent* AudioComponent;

	UPROPERTY(EditDefaultsOnly, Category = "Sound")
		USoundBase* HitSound;

	UPROPERTY(EditDefaultsOnly, Category = "Sound")
		USoundBase* DeathSound;

	UPROPERTY(EditDefaultsOnly, Category = "Sound")
		USoundBase* BlockSound;

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
		
	
	//Color getters
	UFUNCTION(BlueprintPure)
		FORCEINLINE FLinearColor GetVFXLowNeutral() const { return LowNeutralColor; }
	UFUNCTION(BlueprintPure)
		FORCEINLINE FLinearColor GetVFXHighNeutral() const { return HighNeutralColor; }
		
	UFUNCTION(BlueprintPure)
		FORCEINLINE FLinearColor GetVFXLowAttack1() const { return LowAttack1Color; }
	UFUNCTION(BlueprintPure)
		FORCEINLINE FLinearColor GetVFXHighAttack1() const { return HighAttack1Color; }
		
	UFUNCTION(BlueprintPure)
		FORCEINLINE FLinearColor GetVFXLowAttack2() const { return LowAttack2Color; }
	UFUNCTION(BlueprintPure)
		FORCEINLINE FLinearColor GetVFXHighAttack2() const { return HighAttack2Color; }
		
	UFUNCTION(BlueprintPure)
		FORCEINLINE FLinearColor GetVFXLowAttack3() const { return LowAttack3Color; }
	UFUNCTION(BlueprintPure)
		FORCEINLINE FLinearColor GetVFXHighAttack3() const { return HighAttack3Color; }
};