// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "Combo.h"
#include "PlayerCharacter.generated.h"

class UInputAction;
class USpringArmComponent;
class UCameraComponent;
class UComboManagerComponent;

UCLASS()
class BEATTHEBEATS_API APlayerCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	APlayerCharacter();

	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

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
	TObjectPtr<UInputAction> NeautralAttackAction;

	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<UInputAction> Type1AttackAction;

	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<UInputAction> Type2AttackAction;

	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<UInputAction> Type3AttackAction;

	/**
	* Callbacks for input
	*/
	void Move(const FInputActionValue& Value);
	void Look(const FInputActionValue& Value);
	void AddNeutralAttack();
	void AddType1Attack();
	void AddType2Attack();
	void AddType3Attack();

private:
	void AttackCallback(Attacks AttackType, float MotionValue, float AnimLength, int Combo, int ComboStep);

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USpringArmComponent> CameraBoom;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UCameraComponent> ViewCamera;

};
