// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Enemy/Enemy.h"
#include "Boss.generated.h"

UENUM(BlueprintType)
enum class EBossState : uint8
{
	EBS_Chasing UMETA(DisplayName = "Chasing"),
	EBS_Attacking UMETA(DisplayName = "Attacking"),
};

class UAnimMontage;

UCLASS()
class BEATTHEBEATS_API ABoss : public AEnemy
{
	GENERATED_BODY()

public:
	ABoss();
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

protected:
	virtual void BeginPlay() override;

	virtual void OnBeat(float CurrentTimeSinceLastBeat) override;

	virtual void Attack() override;

	void PlayAttackMontage();

	virtual void DoDamage() override;

	UPROPERTY(BlueprintReadWrite)
	EBossState BossState;

private:
	UPROPERTY(EditDefaultsOnly, Category = "Montages")
	TObjectPtr<UAnimMontage> AttackMontage;

	APawn* Player;

public:	


};
