// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/RangedEnemy.h"

#include "Kismet/GameplayStatics.h"
#include "Engine/DamageEvents.h"
#include "Beats/BeatManager.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"

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

}

void ARangedEnemy::OnBeat(float CurrentTimeSinceLastBeat)
{
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

void ARangedEnemy::Attack()
{
	Super::Attack();

	if (CurrentAttack < StandardCombo.AttackCount() - 1) {
		FVector direction = UGameplayStatics::GetPlayerPawn(this, 0)->GetActorLocation() - GetActorLocation();
		FRotator rotation = direction.Rotation();

		SetActorRotation(rotation);

		AddLaserBeam(UGameplayStatics::GetPlayerPawn(this, 0)->GetActorLocation());
	}
	else {
		DoDamage();
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

		NiagaraComp->SetNiagaraVariableFloat(FString("LifeTime"), BeatManager->TimeBetweenBeats());
		NiagaraComp->SetNiagaraVariableFloat(FString("Width"), LaserWidth);
		NiagaraComp->SetVariablePosition(FName("BeamEnd"), end);
	}
}