// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapons/WeaponBase.h"
#include "Character/PlayerCharacter.h"
#include "Components/BoxComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Interfaces/HitInterface.h"
#include "Combos/QTEProjectile.h"
#include "Beats/BeatManager.h"

AWeaponBase::AWeaponBase()
{
	PrimaryActorTick.bCanEverTick = true;

	ItemMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ItemMeshComponent"));
	RootComponent = ItemMesh;

	WeaponBox = CreateDefaultSubobject<UBoxComponent>(TEXT("WeaponBox"));
	WeaponBox->SetupAttachment(GetRootComponent());
	WeaponBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	WeaponBox->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Overlap);
	WeaponBox->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore); 

	BoxTraceStart = CreateDefaultSubobject<USceneComponent>(TEXT("Box Trace Start"));
	BoxTraceStart->SetupAttachment(GetRootComponent());
	BoxTraceEnd = CreateDefaultSubobject<USceneComponent>(TEXT("Box Trace End"));
	BoxTraceEnd->SetupAttachment(GetRootComponent());
}

void AWeaponBase::BeginPlay()
{
	Super::BeginPlay();

	QueryType = UEngineTypes::ConvertToTraceType(ECollisionChannel::ECC_GameTraceChannel3);
	
	WeaponBox->OnComponentBeginOverlap.AddDynamic(this, &AWeaponBase::OnBoxOverlap);

	BeatManager = Cast<ABeatManager>(UGameplayStatics::GetActorOfClass(this, BeatManagerClass));

	if (BeatManager == nullptr) {
		UE_LOG(LogTemp, Error, TEXT("No Beat Manager was found in the scene!"));
	}
}

void AWeaponBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AWeaponBase::SetAttackStatus(float Damage, Attacks AttackType, bool OnBeat)
{
	CurrentDamage = Damage;
	CurrentAttackType = AttackType;
	CurrentlyOnBeat = OnBeat;
}

void AWeaponBase::SpawnProjectile(TSubclassOf<AQTEProjectile> projectile, FVector position, FRotator rotation)
{
	AQTEProjectile* bullet = GetWorld()->SpawnActor<AQTEProjectile>(projectile, FVector(0, 0, 100000), FRotator(0, rotation.Yaw, 0));

	if (bullet && BeatManager) {
		bullet->SetAttackValues(CurrentDamage, CurrentAttackType, BeatManager->GetTimeUntilNextBeat() + BeatManager->TimeBetweenBeats(), position);
	}
}

void AWeaponBase::OnBoxOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	const FVector Start = BoxTraceStart->GetComponentLocation();
	const FVector End = BoxTraceEnd->GetComponentLocation();

	TArray<AActor*> ActorsToIgnore;
	for (AActor* Actor : IgnoreActors)
	{
		ActorsToIgnore.AddUnique(Actor);
	}

	FHitResult BoxHit;

	UKismetSystemLibrary::BoxTraceSingle(
		this,
		Start,
		End,
		BoxTraceHalfSize,
		BoxTraceStart->GetComponentRotation(),
		QueryType,
		false,
		ActorsToIgnore,
		EDrawDebugTrace::None,
		BoxHit,
		true
	);

	if (BoxHit.GetActor() != nullptr)
	{
		IHitInterface* HitInterface = Cast<IHitInterface>(BoxHit.GetActor());
		if (HitInterface != nullptr)
		{
			float finalDamage = HitInterface->ApplyDamage(CurrentDamage, CurrentAttackType, CurrentlyOnBeat, BoxHit.ImpactPoint);

			if (finalDamage > CurrentDamage || CurrentAttackType == Attacks::Attack_Guaranteed) {
				HitInterface->GetHit(BoxHit.ImpactPoint);
			}
		}
		IgnoreActors.AddUnique(BoxHit.GetActor());
	}
}

