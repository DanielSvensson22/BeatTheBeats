// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/EnemyProjectile.h"
#include "NiagaraSystem.h"
#include "NiagaraFunctionLibrary.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Beats/BeatManager.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Engine/DamageEvents.h"

// Sets default values
AEnemyProjectile::AEnemyProjectile()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Collider = CreateDefaultSubobject<UCapsuleComponent>(TEXT("Capsule Collider"));
	Collider->SetCapsuleRadius(50);
	Collider->SetCapsuleHalfHeight(50);
	Collider->SetGenerateOverlapEvents(true);
	SetRootComponent(Collider);

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
	Mesh->SetupAttachment(RootComponent);
}

// Called when the game starts or when spawned
void AEnemyProjectile::BeginPlay()
{
	Super::BeginPlay();
	
	Collider->OnComponentBeginOverlap.AddDynamic(this, &AEnemyProjectile::OnOverlapBegin);
	Collider->OnComponentEndOverlap.AddDynamic(this, &AEnemyProjectile::OnOverlapEnd);

	BeatManager = Cast<ABeatManager>(UGameplayStatics::GetActorOfClass(this, BeatManagerClass));

	if (BeatManager == nullptr) {
		UE_LOG(LogTemp, Error, TEXT("No Beat Manager was found in the scene!"));
	}

	AttackTypeMaterial = Mesh->CreateDynamicMaterialInstance(0, Mesh->GetMaterial(0));

	QueryType = UEngineTypes::ConvertToTraceType(ECollisionChannel::ECC_GameTraceChannel1);
}

// Called every frame
void AEnemyProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	CurrentTime += DeltaTime;

	if (BeatManager) {
		FVector location = FMath::VInterpTo(Start, End, CurrentTime, CurrentTime / (BeatsUntilImpact * (BeatManager->TimeBetweenBeats() * 0.6f)));

		if (location == End) {
			FHitResult hit;
			SetActorLocation(location, true, &hit, ETeleportType::TeleportPhysics);

			Explode();

			Destroy();
		}
		else {
			location.Z = FMath::Sin(FMath::DegreesToRadians((CurrentTime / (BeatsUntilImpact * (BeatManager->TimeBetweenBeats() * 0.6f))) * 180)) * CurveHeight;

			FHitResult hit;
			SetActorLocation(location, true, &hit, ETeleportType::TeleportPhysics);
		}
	}
}

void AEnemyProjectile::SetupProjectile(FVector start, FVector end, float damage, Attacks attackType)
{
	Start = start;
	End = end;
	Damage = damage;
	AttackType = attackType;

	if (Mesh && AttackTypeMaterial) {

		switch (AttackType) {
		case Attacks::Attack_Neutral:
			AttackTypeMaterial->SetVectorParameterValue(TEXT("Color"), NeutralColor);
			break;

		case Attacks::Attack_Type1:
			AttackTypeMaterial->SetVectorParameterValue(TEXT("Color"), AttackOneColor);
			break;

		case Attacks::Attack_Type2:
			AttackTypeMaterial->SetVectorParameterValue(TEXT("Color"), AttackTwoColor);
			break;

		case Attacks::Attack_Type3:
			AttackTypeMaterial->SetVectorParameterValue(TEXT("Color"), AttackThreeColor);
			break;

		default:
			AttackTypeMaterial->SetVectorParameterValue(TEXT("Color"), NeutralColor);
			break;
		}

		Mesh->SetMaterial(0, AttackTypeMaterial);
	}
}

void AEnemyProjectile::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor && (OtherActor != this) && OtherComp) {
		Explode();

		Destroy();
	}
}

void AEnemyProjectile::OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	//Not needed right now.
}

void AEnemyProjectile::Explode()
{
	if (!bIsDead) {
		if (ImpactEffect) {
			UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, ImpactEffect, GetActorLocation());

			TArray<AActor*> ActorsToIgnore;
			ActorsToIgnore.Add(this);

			TArray<FHitResult> Hits;

			FVector backward = -(GetActorForwardVector());

			UKismetSystemLibrary::SphereTraceMulti(this, GetActorLocation(), GetActorLocation() + GetActorForwardVector(), Radius, QueryType, false, ActorsToIgnore,
				EDrawDebugTrace::None, Hits, true);

			for (auto& hit : Hits) {
				if (AActor* obj = hit.GetActor()) {
					if (!ActorsToIgnore.Contains(obj)) {
						FPointDamageEvent DamageEvent(Damage, hit, backward, nullptr);
						obj->TakeDamage(Damage, DamageEvent, GetInstigatorController(), this);

						ActorsToIgnore.Add(obj);
					}
				}
			}
		}

		bIsDead = true;
	}
}

