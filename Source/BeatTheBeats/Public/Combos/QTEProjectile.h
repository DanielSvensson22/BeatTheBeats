// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Combo.h"
#include "QTEProjectile.generated.h"

class UNiagaraSystem;
class UCapsuleComponent;
class UNiagaraComponent;

UCLASS()
class BEATTHEBEATS_API AQTEProjectile : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AQTEProjectile();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void SetAttackValues(float damage, Attacks attackType, float LifeTime, FVector StartPosition);

	// Overlap begin function
	UFUNCTION()
		void OnOverlapBegin(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor,
			class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	// Overlap end function
	UFUNCTION()
		void OnOverlapEnd(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor,
			class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

private:

	void Kill();

private:

	UPROPERTY(EditDefaultsOnly)
	bool bCausesPushBack = false;

	UPROPERTY(EditDefaultsOnly)
	UNiagaraSystem* VFX;

	UNiagaraComponent* VFXComp;

	UPROPERTY(VisibleAnywhere)
	UCapsuleComponent* Collider;

	UPROPERTY(EditDefaultsOnly)
	float PushbackForce = 500;

	UPROPERTY(EditDefaultsOnly)
	float Speed = 100;

	UPROPERTY(EditDefaultsOnly)
	FLinearColor EffectColor = FLinearColor::Black;

	UPROPERTY(EditDefaultsOnly)
	float ParticleSpawnRate = 30;

	UPROPERTY(EditDefaultsOnly)
	float ParticleScale = 100;

	UPROPERTY(EditDefaultsOnly)
	float LifeMultiplier = 1;

	UPROPERTY(EditDefaultsOnly)
	float RotationOffset = 0;

	float Damage;

	Attacks AttackType;

	TArray<AActor*> HitEnemies;
};
