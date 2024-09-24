// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/Enemy.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/CapsuleComponent.h"
#include "Engine/DamageEvents.h"
#include "Kismet/GameplayStatics.h"

AEnemy::AEnemy()
{
	PrimaryActorTick.bCanEverTick = true;

	GetMesh()->SetCollisionObjectType(ECollisionChannel::ECC_Pawn);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
	GetMesh()->SetGenerateOverlapEvents(true);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
}

void AEnemy::BeginPlay()
{
	Super::BeginPlay();
	
}

void AEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AEnemy::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void AEnemy::OnBeat(float CurrentTimeSinceLastBeat)
{
	Super::OnBeat(CurrentTimeSinceLastBeat);

	if (CurrentAttack > -1) {
		Attack();
	}
}

void AEnemy::Attack()
{
	Super::Attack();

	DoDamage();
}

void AEnemy::DoDamage()
{
	if (CurrentAttack == StandardCombo.AttackCount() - 1) {
		FVector start;
		FRotator rotation;

		GetController()->GetPlayerViewPoint(start, rotation);

		FVector forward = rotation.Vector();

		FVector end = start + forward * AttackRange;

		FHitResult result;
		FCollisionQueryParams params;
		params.AddIgnoredActor(this);

		if (GetWorld()->LineTraceSingleByChannel(result, start, end, ECollisionChannel::ECC_GameTraceChannel1, params)) {
			if (HitEffect) {
				UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), HitEffect, result.Location, (-forward).Rotation());
			}

			FPointDamageEvent DamageEvent(Damage, result, -forward, nullptr);

			if (AActor* hitActor = result.GetActor()) {
				hitActor->TakeDamage(Damage, DamageEvent, GetInstigatorController(), this);
			}
		}
	}
}
