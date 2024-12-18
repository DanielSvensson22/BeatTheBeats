// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/MeleeEnemy.h"
#include "Beats/BeatManager.h"
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

float AMeleeEnemy::ApplyDamage(float InitialDamage, Attacks AttackType, bool OnBeat, FVector HitLocation)
{
	float finalDamage = Super::ApplyDamage(InitialDamage, AttackType, OnBeat, HitLocation);

	if (!bHasEscapedCombat && IsAlive() && CurrentHealth < MaxHealth / 2) {
		ExitQueue();
		SetAttackState(false, false);

		bHasEscapedCombat = true;
	}

	return finalDamage;
}

void AMeleeEnemy::BeginPlay()
{
	Super::BeginPlay();

	if (bRandomizeType) {
		int rand = FMath::RandRange(0, 3);

		switch (rand) {
		case 0:
			EnemyType = Attacks::Attack_Neutral;
			SetEffectsColor(EnemyType);
			break;

		case 1:
			EnemyType = Attacks::Attack_Type1;
			SetEffectsColor(EnemyType);
			break;

		case 2:
			EnemyType = Attacks::Attack_Type2;
			SetEffectsColor(EnemyType);
			break;

		case 3:
			EnemyType = Attacks::Attack_Type3;
			SetEffectsColor(EnemyType);
			break;

		default:
			EnemyType = Attacks::Attack_Neutral;
			SetEffectsColor(EnemyType);
			break;
		}
	}
}

void AMeleeEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AMeleeEnemy::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void AMeleeEnemy::PerformAttackAnimation()
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();

	if (AnimInstance) {
		AnimInstance->StopAllMontages(true);
	}

	PlayEnemyMontage(AttackAnim, TEXT("Atk"), BeatManager->TimeBetweenBeats());
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
	if (CurrentAttack == StandardCombo.AttackCount() - 2)
	{
		PlayEnemyMontage(AttackAnim, TEXT("ReadyAttack"), BeatManager->TimeBetweenBeats());

		FTimerHandle handle;

		GetWorldTimerManager().SetTimer(handle, this, &AMeleeEnemy::PerformAttackAnimation, BeatManager->TimeBetweenBeats() * 0.5f, false);
	}

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