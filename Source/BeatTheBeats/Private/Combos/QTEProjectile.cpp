// Fill out your copyright notice in the Description page of Project Settings.


#include "Combos/QTEProjectile.h"
#include "NiagaraSystem.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "Components/CapsuleComponent.h"
#include "Enemy/EnemyBase.h"
#include "Interfaces/HitInterface.h"
#include "TimerManager.h"

// Sets default values
AQTEProjectile::AQTEProjectile()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Collider = CreateDefaultSubobject<UCapsuleComponent>(TEXT("Capsule Collider"));
	Collider->SetCapsuleRadius(50);
	Collider->SetCapsuleHalfHeight(50);
	Collider->SetGenerateOverlapEvents(true);
	SetRootComponent(Collider);
}

// Called when the game starts or when spawned
void AQTEProjectile::BeginPlay()
{
	Super::BeginPlay();
	
	VFXComp = UNiagaraFunctionLibrary::SpawnSystemAttached(VFX, RootComponent, TEXT("Capsule Collider"), 
															FVector::ZeroVector, FRotator::ZeroRotator, EAttachLocation::KeepRelativeOffset, false);
	VFXComp->Activate(true);

	Collider->OnComponentBeginOverlap.AddDynamic(this, &AQTEProjectile::OnOverlapBegin);
	Collider->OnComponentEndOverlap.AddDynamic(this, &AQTEProjectile::OnOverlapEnd);
}

// Called every frame
void AQTEProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	FHitResult result;
	SetActorLocation(GetActorLocation() + Speed * DeltaTime * GetActorForwardVector(), true, &result, ETeleportType::TeleportPhysics);
}

void AQTEProjectile::SetAttackValues(float damage, Attacks attackType, float LifeTime, FVector StartPosition)
{
	Damage = damage;
	AttackType = attackType;

	VFXComp->SetVariableLinearColor(TEXT("Color"), EffectColor);
	VFXComp->SetVariableFloat(TEXT("Radius"), Collider->GetScaledCapsuleRadius());
	VFXComp->SetVariableFloat(TEXT("SpawnRate"), ParticleSpawnRate);
	VFXComp->SetVariableFloat(TEXT("Scale"), ParticleScale);

	FHitResult result;
	SetActorLocation(StartPosition, true, &result, ETeleportType::TeleportPhysics);

	FTimerHandle handle;
	GetWorldTimerManager().SetTimer(handle, this, &AQTEProjectile::Kill, LifeTime * LifeMultiplier, false);
}

void AQTEProjectile::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor && (OtherActor != this) && OtherComp) {

		if (!HitEnemies.Contains(OtherActor)) {
			if (IHitInterface* enemy = Cast<IHitInterface>(OtherActor)) {
				enemy->GetHit(SweepResult.ImpactPoint);
				enemy->ApplyDamage(Damage, AttackType, true, SweepResult.ImpactPoint);

				if (bCausesPushBack) {
					enemy->ApplyPushBack(PushbackForce);
				}

				HitEnemies.Emplace(OtherActor);

				if (Damage <= 0) {
					UE_LOG(LogTemp, Error, TEXT("Projectile damage was 0!"));
				}
			}
		}
	}
}

void AQTEProjectile::OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	//Not needed right now.
}

void AQTEProjectile::Kill()
{
	Destroy(false, true);
}

