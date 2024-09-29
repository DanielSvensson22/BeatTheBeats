// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/EnemyBase.h"
#include "Kismet/GameplayStatics.h"
#include "Beats/BeatManager.h"
#include "Enemy/EnemyQueue.h"
#include "Components/CapsuleComponent.h"

// Sets default values
AEnemyBase::AEnemyBase()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	GetMesh()->SetCollisionObjectType(ECollisionChannel::ECC_Pawn);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
	GetMesh()->SetGenerateOverlapEvents(true);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
}

// Called when the game starts or when spawned
void AEnemyBase::BeginPlay()
{
	Super::BeginPlay();

	CurrentHealth = MaxHealth;
	
	BeatManager = Cast<ABeatManager>(UGameplayStatics::GetActorOfClass(this, BeatManagerClass));

	if (BeatManager == nullptr) {
		UE_LOG(LogTemp, Error, TEXT("No Beat Manager was found in the scene!"));
	}
	else {
		BeatManager->BindFuncToOnBeat(this, &AEnemyBase::OnBeat);
	}

	EnemyQueue = Cast<AEnemyQueue>(UGameplayStatics::GetActorOfClass(this, EnemyQueueClass));

	if (EnemyQueue == nullptr) {
		UE_LOG(LogTemp, Error, TEXT("Enemy queue was not found by the AI!"));
	}

	Player = UGameplayStatics::GetPlayerPawn(this, 0);

	if (Player == nullptr) {
		UE_LOG(LogTemp, Error, TEXT("Player pawn was not found by the AI!"));
	}
}

void AEnemyBase::OnBeat(float CurrentTimeSinceLastBeat)
{
	if (bCanAttack) {
		CurrentAttack = StandardCombo.NextAttack();
	}
	else {
		CurrentAttack = StandardCombo.ResetCombo();
	}
}

void AEnemyBase::Attack()
{
	FString attack;

	switch (StandardCombo.GetAttackType(CurrentAttack)) {
	case Attacks::Attack_Neutral:
		attack = "Neutral";
		break;

	case Attacks::Attack_Type1:
		attack = "Type 1";
		break;

	case Attacks::Attack_Type2:
		attack = "Type 2";
		break;

	case Attacks::Attack_Type3:
		attack = "Type 3";
		break;

	default:
		attack = "Not Implemented";
		break;
	}

	UE_LOG(LogTemp, Display, TEXT("Enemy performed %s attack on combo step %i"), *attack, CurrentAttack);
}

// Called every frame
void AEnemyBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AEnemyBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

bool AEnemyBase::GetCanAttack()
{
	return bCanAttack;
}

void AEnemyBase::SetAttackState(bool CanAttack, bool CanChase)
{
	bCanAttack = CanAttack;
	bCanChasePlayer = CanChase;
}

void AEnemyBase::EnterQueue()
{
	if (!bHasAddedToQueue) {
		if (EnemyQueue) {
			EnemyQueue->AddToQueue(this);
			bHasAddedToQueue = true;
		}
	}
}

void AEnemyBase::DoDamage()
{

}

