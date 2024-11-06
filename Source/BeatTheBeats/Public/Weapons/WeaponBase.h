// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "../Combos/Combo.h"
#include "WeaponBase.generated.h"

class UBoxComponent;

UCLASS()
class BEATTHEBEATS_API AWeaponBase : public AActor
{
	GENERATED_BODY()
	
public:	
	AWeaponBase();
	virtual void Tick(float DeltaTime) override;

	void SetAttackStatus(float Damage, Attacks AttackType, bool OnBeat);

	// Actors to Ignore during an Overlaping Event
	TArray<AActor*> IgnoreActors;

protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	void OnBoxOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UStaticMeshComponent> ItemMesh;

	UPROPERTY(VisibleAnywhere, Category = "Weapon Properties")
	TObjectPtr<UBoxComponent> WeaponBox;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USceneComponent> BoxTraceStart;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USceneComponent> BoxTraceEnd;

	//Combat info:
	float CurrentDamage = 0;
	Attacks CurrentAttackType = Attacks::Attack_Neutral;
	bool CurrentlyOnBeat = false;

	UPROPERTY(EditDefaultsOnly)
	FVector BoxTraceHalfSize = FVector(7.5f, 7.5f, 7.5f);

	ETraceTypeQuery QueryType;

public:
	FORCEINLINE TObjectPtr<UBoxComponent> GetWeaponBox() const { return WeaponBox; }
};
