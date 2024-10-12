// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/MeleeEnemy.h"

#include "Components/CapsuleComponent.h"
#include "Engine/DamageEvents.h"
#include "Kismet/GameplayStatics.h"

AMeleeEnemy::AMeleeEnemy() : Super()
{
	AttackPoint = CreateDefaultSubobject<USceneComponent>(TEXT("Attack point"));
	AttackPoint->SetupAttachment(RootComponent);

	bIsMelee = true;
}

bool AMeleeEnemy::GetCanAttack()
{
	LastDistanceToPlayer = FVector::Dist(AttackPoint->GetComponentLocation(), UGameplayStatics::GetPlayerPawn(this, 0)->GetActorLocation());
	if (LastDistanceToPlayer < AttackRange) {
		return true;
	}

	return bCanAttack;
}

void AMeleeEnemy::BeginPlay()
{
	Super::BeginPlay();

}

void AMeleeEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AMeleeEnemy::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void AMeleeEnemy::OnBeat(float CurrentTimeSinceLastBeat)
{
	if (bIsStunned) {
		CurrentAttack = StandardCombo.ResetCombo();
		bIsStunned = false;
	}
	else {
		if (bCanAttack || LastDistanceToPlayer < AttackRange) {
			CurrentAttack = StandardCombo.NextAttack();
		}
		else {
			CurrentAttack = StandardCombo.ResetCombo();
		}

		if (CurrentAttack > -1) {
			Attack();
		}
	}
}

void AMeleeEnemy::Attack()
{
	DoDamage();
}

void AMeleeEnemy::DoDamage()
{
	if (CurrentAttack == StandardCombo.AttackCount() - 1) {
		FVector start;
		FRotator rotation;

		GetController()->GetPlayerViewPoint(start, rotation);
		start = AttackPoint->GetComponentLocation();

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