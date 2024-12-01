// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/RangedEnemy.h"

#include "Kismet/GameplayStatics.h"
#include "Engine/DamageEvents.h"
#include "Beats/BeatManager.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"
#include "Components/AudioComponent.h"

ARangedEnemy::ARangedEnemy() : Super()
{
	ShootPoint = CreateDefaultSubobject<USceneComponent>(TEXT("Attack point"));
	ShootPoint->SetupAttachment(RootComponent);

	bIsMelee = false;
}

bool ARangedEnemy::GetCanAttack()
{
	if (FVector::Dist(ShootPoint->GetComponentLocation(), UGameplayStatics::GetPlayerPawn(this, 0)->GetActorLocation())
		< CloseQuarterDistance) {
		return true;
	}

	return bCanAttack;
}

void ARangedEnemy::BeginPlay()
{
	Super::BeginPlay();

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

void ARangedEnemy::OnBeat(float CurrentTimeSinceLastBeat)
{
	if (bIsStunned) {
		CurrentAttack = StandardCombo.ResetCombo();
		bIsStunned = false;
	}
	else {
		if (GetCanAttack()) {
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

void ARangedEnemy::Attack()
{
	if (CurrentAttack < StandardCombo.AttackCount() - 1) {

		FHitResult result;
		FCollisionQueryParams params;
		params.AddIgnoredActor(this);

		if (GetWorld()->LineTraceSingleByChannel(result, GetActorLocation(), UGameplayStatics::GetPlayerPawn(this, 0)->GetActorLocation(), ECollisionChannel::ECC_Visibility, params)) {
			FVector direction = result.Location - GetActorLocation();
			FRotator rotation = direction.Rotation();

			SetActorRotation(rotation);

			AddLaserBeam(result.Location);
		}

		if (CurrentAttack == StandardCombo.AttackCount() - 2) {
			if (ChargeUpSound) {
				AudioComponent->SetSound(ChargeUpSound);
				AudioComponent->Play();
			}
		}
	}
	else {
		DoDamage();

		if (ShootSound) {
			AudioComponent->SetSound(ShootSound);
			AudioComponent->Play();
		}
	}
}

void ARangedEnemy::DoDamage()
{
	FVector start;
	FRotator rotation;

	GetController()->GetPlayerViewPoint(start, rotation);
	start = ShootPoint->GetComponentLocation();

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

		AddLaserBeam(result.Location);
	}
	else {

		AddLaserBeam(end);
	}

	if (MuzzleEffect) {
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), MuzzleEffect, ShootPoint->GetComponentLocation(), (-forward).Rotation());
	}

	ExitQueue();
	SetAttackState(false, false);
}

void ARangedEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ARangedEnemy::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void ARangedEnemy::AddLaserBeam(const FVector& end)
{
	if (LaserBeam) {
		UNiagaraComponent* NiagaraComp = UNiagaraFunctionLibrary::SpawnSystemAttached(LaserBeam, ShootPoint,
			NAME_None, FVector(0.f), FRotator(0.f), EAttachLocation::Type::KeepRelativeOffset, true);

		NiagaraComp->SetVariableFloat(FName("LifeTime"), BeatManager->TimeBetweenBeats());
		NiagaraComp->SetVariableFloat(FName("Width"), LaserWidth);
		NiagaraComp->SetVariablePosition(FName("BeamEnd"), end);
	}
}