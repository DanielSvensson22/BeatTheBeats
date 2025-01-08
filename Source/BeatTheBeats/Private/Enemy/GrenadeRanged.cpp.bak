// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/GrenadeRanged.h"
#include "Components/AudioComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Enemy/EnemyProjectile.h"

void AGrenadeRanged::Attack()
{
	FVector playerLocation = UGameplayStatics::GetPlayerPawn(this, 0)->GetActorLocation();

	FVector direction = playerLocation - GetActorLocation();
	FRotator rotation = direction.Rotation();

	SetActorRotation(rotation);

	if (CurrentAttack == StandardCombo.AttackCount() - 1) {
		DoDamage();

		if (ShootSound) {
			AudioComponent->SetSound(ShootSound);
			AudioComponent->Play();
		}
	}
	else {
		return;
	}
}

void AGrenadeRanged::DoDamage()
{
	FVector start;
	FRotator rotation;

	GetController()->GetPlayerViewPoint(start, rotation);
	start = ShootPoint->GetComponentLocation();

	FVector forward = rotation.Vector();

	FVector playerLocation = UGameplayStatics::GetPlayerPawn(this, 0)->GetActorLocation();

	AEnemyProjectile* projectile = GetWorld()->SpawnActor<AEnemyProjectile>(ProjectileClass, start, FRotator::ZeroRotator);

	if (projectile) {
		projectile->SetupProjectile(start, playerLocation, Damage, EnemyType);
	}

	if (MuzzleEffect) {
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), MuzzleEffect, ShootPoint->GetComponentLocation(), (-forward).Rotation());
	}

	ExitQueue();
	SetAttackState(false, false);
}
