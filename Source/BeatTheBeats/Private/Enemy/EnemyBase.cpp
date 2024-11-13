// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/EnemyBase.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Beats/BeatManager.h"
#include "Enemy/EnemyQueue.h"
#include "Components/CapsuleComponent.h"
#include "Components/WidgetComponent.h"
#include "Score/ScoreManager.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"
#include "Camera/BBCameraShake.h"

// Sets default values
AEnemyBase::AEnemyBase()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	GetMesh()->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel3, ECollisionResponse::ECR_Block);
	GetMesh()->SetGenerateOverlapEvents(true);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel3, ECollisionResponse::ECR_Block);

	EnemyWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("Enemy Widget"));
	EnemyWidget->SetWidgetSpace(EWidgetSpace::Screen);
	EnemyWidget->SetupAttachment(RootComponent);
	EnemyWidget->AddLocalOffset(FVector(0, 0, GetCapsuleComponent()->GetScaledCapsuleHalfHeight()));
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
		BeatHandle = BeatManager->BindFuncToOnBeat(this, &AEnemyBase::OnBeat);
	}

	EnemyQueue = Cast<AEnemyQueue>(UGameplayStatics::GetActorOfClass(this, EnemyQueueClass));

	if (EnemyQueue == nullptr) {
		UE_LOG(LogTemp, Error, TEXT("Enemy queue was not found by the AI!"));
	}

	Player = UGameplayStatics::GetPlayerPawn(this, 0);

	if (Player == nullptr) {
		UE_LOG(LogTemp, Error, TEXT("Player pawn was not found by the AI!"));
	}

	ScoreManager = Cast<AScoreManager>(UGameplayStatics::GetActorOfClass(this, ScoreManagerClass));

	if (ScoreManager == nullptr) {
		UE_LOG(LogTemp, Error, TEXT("No Score Manager was found in the scene!"));
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


void AEnemyBase::PlayHitReactMontage(const FName& SectionName)
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance != nullptr && HitReactMontage != nullptr)
	{
		AnimInstance->Montage_Play(HitReactMontage);
		AnimInstance->Montage_JumpToSection(SectionName, HitReactMontage);
	}
}

void AEnemyBase::GetHit(const FVector& ImpactPoint)
{
	if (GetWorld()) DrawDebugSphere(GetWorld(), ImpactPoint, 10.f, 12, FColor::Orange, false, 5.f);

	DirectionalHitReact(ImpactPoint);

	bIsStunned = true;
}

void AEnemyBase::DirectionalHitReact(const FVector& ImpactPoint)
{
	const FVector Forward = GetActorForwardVector();
	//Lower Impact Point to the Enemy's Actor Location Z
	const FVector ImpactLower(ImpactPoint.X, ImpactPoint.Y, GetActorLocation().Z);
	FVector ToHit = (ImpactLower - GetActorLocation()).GetSafeNormal();
	// Forward * ToHit = |Forward||ToHit| * cos(theta)
	// |Forward| && |ToHit| = 1, so Forward * Tohit = cos(theta)
	const double CosTheta = FVector::DotProduct(Forward, ToHit);
	double Theta = FMath::Acos(CosTheta);
	//Convert from radians to degrees
	Theta = FMath::RadiansToDegrees(Theta);
	// If Crossproduct points down, Theta should be negative
	const FVector CrossProduct = FVector::CrossProduct(Forward, ToHit);
	if (CrossProduct.Z < 0) { Theta *= -1.f; }

	// Condition for which AM to Play
	FName SectionName("FromBack");
	if (Theta >= -45.f && Theta < 45.f) { SectionName = FName("FromFront"); }
	else if (Theta >= -135.f && Theta < -45.f) { SectionName = FName("FromLeft"); }
	else if (Theta >= 45.f && Theta < 135.f) { SectionName = FName("FromRight"); }

	PlayHitReactMontage(FName(SectionName));

	/*
	** Draw debug line for calculation vector
	*/
	if (GEngine != nullptr) { GEngine->AddOnScreenDebugMessage(1, 5.f, FColor::Green, FString::Printf(TEXT("Theta: %f"), Theta)); }
	UKismetSystemLibrary::DrawDebugArrow(this, GetActorLocation(), GetActorLocation() + CrossProduct * 100.f, 5.f, FColor::Blue, 5.f);
	UKismetSystemLibrary::DrawDebugArrow(this, GetActorLocation(), GetActorLocation() + Forward * 100.f, 5.f, FColor::Red, 5.f);
	UKismetSystemLibrary::DrawDebugArrow(this, GetActorLocation(), GetActorLocation() + ToHit * 100.f, 5.f, FColor::Green, 5.f);
}

void AEnemyBase::ApplyDamage(float InitialDamage, Attacks AttackType, bool OnBeat, FVector HitLocation)
{
	float FinalDamage = InitialDamage;

	if (AttackType == Attacks::Attack_Neutral) {
		//Change nothing.
	}
	else if (AttackType == EnemyType) {
		FinalDamage *= OptimalAttackMultiplier;
	}
	else {
		FinalDamage /= OptimalAttackMultiplier;
	}

	if (OnBeat) {
		FinalDamage *= OptimalAttackMultiplier;
	}
	else {
		FinalDamage /= OptimalAttackMultiplier;
	}

	CurrentHealth = FMath::Clamp(CurrentHealth - FinalDamage, 0, MaxHealth);

	if (GetHitEffect) {
		UNiagaraComponent* NiagaraComp = UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, GetHitEffect, HitLocation, GetActorRotation(), FVector::OneVector, true);

		if (OnBeat && AttackType == EnemyType) {
			NiagaraComp->SetVariableLinearColor(TEXT("Color"), FLinearColor::Red);
		}
		else if (OnBeat || AttackType == EnemyType) {
			NiagaraComp->SetVariableLinearColor(TEXT("Color"), FLinearColor::Yellow);
		}
		else {
			NiagaraComp->SetVariableLinearColor(TEXT("Color"), FLinearColor::Blue);
		}
	}	

	if (!IsAlive()) {
		if (!bHasDied) {
			bHasDied = true;

			ExitQueue();

			DetachFromControllerPendingDestroy();

			GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
			SetActorTickEnabled(false);

			if (BeatManager)
				BeatManager->UnBindFuncFromOnBeat(BeatHandle);

			CurrentAttack = StandardCombo.ResetCombo();

			Death();
		}
	}
	else {
		if (ScoreManager) {
			ScoreManager->AddPoints(FinalDamage);
		}

		//UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0)->StartCameraShake(UBBCameraShake::StaticClass());
		Hit();
	}
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
			if (bIsMelee) {
				EnemyQueue->AddToQueue(this);
			}
			else {
				EnemyQueue->AddToRangedQueue(this);
			}

			bHasAddedToQueue = true;
		}
	}
}

void AEnemyBase::ExitQueue()
{
	if (bHasAddedToQueue) {
		bHasAddedToQueue = false;

		if (EnemyQueue) {
			EnemyQueue->RemoveEnemy(this, bIsMelee);
		}
	}
}

void AEnemyBase::Parry()
{
	FString attack;

	switch (EnemyType) {
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

	UE_LOG(LogTemp, Warning, TEXT("%s attack was parried!"), *attack);

	if (ScoreManager) {
		ScoreManager->UpdateUI();
	}
}

void AEnemyBase::DoDamage()
{

}

void AEnemyBase::Death_Implementation()
{

}

void AEnemyBase::Hit_Implementation()
{

}

