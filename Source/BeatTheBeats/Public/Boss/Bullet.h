// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Bullet.generated.h"

class Player;
class ABoss;
class UProjectileMovementComponent;

UCLASS()
class BEATTHEBEATS_API ABullet : public AActor
{
	GENERATED_BODY()

public:
	ABullet();

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;

	// Bullet's collision component
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class USphereComponent* CollisionComponent;

	// Bullet's projectile movement component
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class UProjectileMovementComponent* ProjectileMovement;

	// Hit effect to spawn on collision
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effects")
	class UParticleSystem* HitEffect;
	// Desired time for the bullet to reach the player
	float TravelTime = 1.0f;

	// Reference to the player (assuming you set this in the bullet's owner class)
	AActor* Player;

	ABoss* Boss;

	// The function to handle the overlap event when bullet hits the player
	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
};