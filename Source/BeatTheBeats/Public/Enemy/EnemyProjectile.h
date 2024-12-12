// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "../Combos/Combo.h"
#include "EnemyProjectile.generated.h"

class UNiagaraSystem;
class UCapsuleComponent;
class UMaterialInstanceDynamic;

//To Do: Trajectory

UCLASS()
class BEATTHEBEATS_API AEnemyProjectile : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AEnemyProjectile();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void SetupProjectile(FVector start, FVector end, float damage, Attacks attackType);

	// Overlap begin function
	UFUNCTION()
		void OnOverlapBegin(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor,
			class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	// Overlap end function
	UFUNCTION()
		void OnOverlapEnd(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor,
			class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	FORCEINLINE Attacks GetAttackType() { return AttackType; }

private:

	void Explode();

private:

	float Damage;

	FVector Start;
	FVector End;

	Attacks AttackType;

	UPROPERTY(EditDefaultsOnly)
	UNiagaraSystem* ImpactEffect;

	UPROPERTY(VisibleAnywhere)
	UCapsuleComponent* Collider;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* Mesh;

	UPROPERTY()
	class ABeatManager* BeatManager;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<ABeatManager> BeatManagerClass;

	UPROPERTY(EditDefaultsOnly)
	int BeatsUntilImpact = 1;

	UPROPERTY(EditDefaultsOnly)
	float Radius = 50;

	UMaterialInstanceDynamic* AttackTypeMaterial;

	UPROPERTY(EditDefaultsOnly, Category = "VFX")
	FLinearColor NeutralColor;

	UPROPERTY(EditDefaultsOnly, Category = "VFX")
	FLinearColor AttackOneColor;

	UPROPERTY(EditDefaultsOnly, Category = "VFX")
	FLinearColor AttackTwoColor;

	UPROPERTY(EditDefaultsOnly, Category = "VFX")
	FLinearColor AttackThreeColor;

	ETraceTypeQuery QueryType;

	float CurrentTime = 0;

	UPROPERTY(EditDefaultsOnly)
	float CurveHeight = 300;

	bool bIsDead = false;
};
