// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/PlayerCharacter.h"
#include "Character/BeatTheBeatsPlayerController.h"
#include "GameFramework\SpringArmComponent.h"
#include "GameFramework\CharacterMovementComponent.h"
#include "Camera/CameraComponent.h"
#include "Combos/ComboManagerComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"
#include "Animation/AnimInstance.h"

#include "Components/InputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "Enemy/EnemyBase.h"
#include "Beats/BeatManager.h"
#include "Weapons/WeaponBase.h"
#include "Components/BoxComponent.h"
#include "Components/CapsuleComponent.h"
#include "../Public/Character/QTEComponent.h"
#include "Score/ScoreManager.h"
#include "Camera/BBCameraShake.h"
#include "NiagaraComponent.h"
#include "NiagaraSystem.h"
#include "NiagaraFunctionLibrary.h"

APlayerCharacter::APlayerCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.f, 600.f, 0.f);

	ECameraState::ECS_FreeCamera;
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(GetRootComponent());
	CameraBoom->AddRelativeLocation(FVector(0.f, 0.f, 50.f));
	CameraBoom->TargetArmLength = 350.f;
	CameraBoom->bEnableCameraRotationLag = true;
	CameraBoom->CameraRotationLagSpeed = 20.f;

	ViewCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("ViewCamera"));
	ViewCamera->SetupAttachment(CameraBoom);

	TargetLockTraceRange = 1000.f;
	TargetLockTraceRadius = 150.f;
	TargetLockMaxMoveDistance = 1500.f;

	ComboManager = CreateDefaultSubobject<UComboManagerComponent>(TEXT("Combo Manager"));
	ComboManager->BindAttackCallbackFunc(this, &APlayerCharacter::AttackCallback);

	QTE = CreateDefaultSubobject<UQTEComponent>(TEXT("QTE Component"));
}

void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	Weapon = GetWorld()->SpawnActor<AWeaponBase>(WeaponClass);
	Weapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::KeepRelativeTransform, TEXT("RightHandSocket"));
	Weapon->SetOwner(this);

	ComboManager->SetWeapon(Weapon);

	CurrentHealth = MaxHealth;

	BeatManager = Cast<ABeatManager>(UGameplayStatics::GetActorOfClass(this, BeatManagerClass));

	if (BeatManager == nullptr) {
		UE_LOG(LogTemp, Error, TEXT("No Beat Manager was found in the scene!"));
	}
	else {
		BeatHandle = BeatManager->BindFuncToOnBeat(this, &APlayerCharacter::OnBeat);
	}

	ScoreManager = Cast<AScoreManager>(UGameplayStatics::GetActorOfClass(this, ScoreManagerClass));

	if (ScoreManager == nullptr) {
		UE_LOG(LogTemp, Error, TEXT("No Score Manager was found in the scene!"));
	}
}

void APlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	RotatePlayerToAttack(DeltaTime);

	SetTargetLockCamera();

	bMovedThisTick = false;
}

void APlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
	{
		EnhancedInputComponent->BindAction(MovementAction, ETriggerEvent::Triggered, this, &APlayerCharacter::Move);
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &APlayerCharacter::Look);
		EnhancedInputComponent->BindAction(TargetLockAction, ETriggerEvent::Started, this, &APlayerCharacter::TargetLock);

		//Attacks
		EnhancedInputComponent->BindAction(NeautralAttackAction, ETriggerEvent::Started, this, &APlayerCharacter::AddNeutralAttack);
		EnhancedInputComponent->BindAction(Type1AttackAction, ETriggerEvent::Started, this, &APlayerCharacter::AddType1Attack);
		EnhancedInputComponent->BindAction(Type2AttackAction, ETriggerEvent::Started, this, &APlayerCharacter::AddType2Attack);
		EnhancedInputComponent->BindAction(Type3AttackAction, ETriggerEvent::Started, this, &APlayerCharacter::AddType3Attack);

		//Blocks
		EnhancedInputComponent->BindAction(NeautralBlockAction, ETriggerEvent::Started, this, &APlayerCharacter::AddNeutralBlock);
		EnhancedInputComponent->BindAction(Type1BlockAction, ETriggerEvent::Started, this, &APlayerCharacter::AddType1Block);
		EnhancedInputComponent->BindAction(Type2BlockAction, ETriggerEvent::Started, this, &APlayerCharacter::AddType2Block);
		EnhancedInputComponent->BindAction(Type3BlockAction, ETriggerEvent::Started, this, &APlayerCharacter::AddType3Block);

		//Debug
		EnhancedInputComponent->BindAction(QTEAction, ETriggerEvent::Started, this, &APlayerCharacter::EnterQTE);
		EnhancedInputComponent->BindAction(QTEAction, ETriggerEvent::Completed, this, &APlayerCharacter::ExitQTE);
		EnhancedInputComponent->BindAction(CameraShakeAction, ETriggerEvent::Started, this, &APlayerCharacter::CameraShake);
		EnhancedInputComponent->BindAction(ParticleAction, ETriggerEvent::Started, this, &APlayerCharacter::SpawnParticle);
	}
}

float APlayerCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	AEnemyBase* enemy = Cast<AEnemyBase>(DamageCauser);

	if (enemy) {
		IncomingAttacks.Emplace(enemy, enemy->GetEnemyType(), DamageAmount);
	}

	return DamageAmount;
}

void APlayerCharacter::SetWeaponCollisionEnabled(ECollisionEnabled::Type CollisionEnable)
{
	if (Weapon != nullptr && Weapon->GetWeaponBox() != nullptr)
	{
		Weapon->GetWeaponBox()->SetCollisionEnabled(CollisionEnable);
		Weapon->IgnoreActors.Empty();
	}
}

void APlayerCharacter::Move(const FInputActionValue& Value)
{
	const FVector2D MovementVector = Value.Get<FVector2D>();

	// Character moving toward where camera is looking
	const FRotator Rotation = Controller->GetControlRotation();
	const FRotator YawRotation(0.f, Rotation.Yaw, 0.f);
	const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

	if (Controller != nullptr)
	{
		AddMovementInput(ForwardDirection, MovementVector.Y);
		AddMovementInput(RightDirection, MovementVector.X);
	}

	bMovedThisTick = true;
}

void APlayerCharacter::Look(const FInputActionValue& Value)
{
	const FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Controller != nullptr && bIsLockingTarget == false)
	{
		AddControllerPitchInput(LookAxisVector.Y);
		AddControllerYawInput(LookAxisVector.X);
	}
}

void APlayerCharacter::TargetLock()
{
	if (bIsLockingTarget)
	{
		ECameraState::ECS_FreeCamera;
		GetCharacterMovement()->bOrientRotationToMovement = true;
		bIsLockingTarget = false;
		TargetLockHitTarget = nullptr;
		bUseControllerRotationYaw = false;
	}
	else
	{
		const FVector Start = GetActorLocation();
		const FRotator CameraRotation = ViewCamera->GetComponentRotation();
		const FVector End = Start + UKismetMathLibrary::GetForwardVector(CameraRotation) * TargetLockTraceRange;

		TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypesArray; // object types to trace
		ObjectTypesArray.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_WorldDynamic));

		TArray<AActor*> IgnoreActors; // leave blank if not needed

		// IgnoreActors.Add(); // Add actors to ingore here if needed

		FHitResult SphereHit;

		UKismetSystemLibrary::SphereTraceSingleForObjects(this, Start, End, TargetLockTraceRadius, ObjectTypesArray, false, IgnoreActors, EDrawDebugTrace::ForDuration, SphereHit, true);

		TargetLockHitTarget = SphereHit.GetActor();

		if (TargetLockHitTarget != nullptr)
		{
			ECameraState::ECS_LockCamera;
			GetCharacterMovement()->bOrientRotationToMovement = false;
			bIsLockingTarget = true;
			bUseControllerRotationYaw = true; // Character look at locked target
		}
	}
}

void APlayerCharacter::SetTargetLockCamera()
{
	if (TargetLockHitTarget != nullptr)
	{
		float Distance = GetDistanceTo(TargetLockHitTarget);
		FVector LockOffset = TargetLockHitTarget->GetActorUpVector() * Distance * LockOffsetModifier;

		GetController()->SetControlRotation(UKismetMathLibrary::FindLookAtRotation(GetActorLocation() + LockOffset, TargetLockHitTarget->GetActorLocation()));

		if (Distance > TargetLockMaxMoveDistance)
		{
			ECameraState::ECS_FreeCamera;
			GetCharacterMovement()->bOrientRotationToMovement = true;
			bIsLockingTarget = false;
			TargetLockHitTarget = nullptr;
			bUseControllerRotationYaw = false;
		}
	}
}

void APlayerCharacter::AddNeutralAttack()
{
	if (bInQTE) {
		QTE->AttemptAttack(Attacks::Attack_Neutral);
	}
	else {
		bool AddedLastBeat = BeatManager->GetCurrentTimeSinceLastBeat() < BeatManager->AfterBeatGrace();
		ComboManager->AddAttack(Attacks::Attack_Neutral, PlayerDamage, !AddedLastBeat);
		UE_LOG(LogTemp, Display, TEXT("Added Neutral Attack to queue."));
	}
}

void APlayerCharacter::AddType1Attack()
{
	if (bInQTE) {
		QTE->AttemptAttack(Attacks::Attack_Type1);
	}
	else {
		bool AddedLastBeat = BeatManager->GetCurrentTimeSinceLastBeat() < BeatManager->AfterBeatGrace();
		ComboManager->AddAttack(Attacks::Attack_Type1, PlayerDamage, !AddedLastBeat);
		UE_LOG(LogTemp, Display, TEXT("Added Type 1 Attack to queue."));
	}
}

void APlayerCharacter::AddType2Attack()
{
	if (bInQTE) {
		QTE->AttemptAttack(Attacks::Attack_Type2);
	}
	else {
		bool AddedLastBeat = BeatManager->GetCurrentTimeSinceLastBeat() < BeatManager->AfterBeatGrace();
		ComboManager->AddAttack(Attacks::Attack_Type2, PlayerDamage, !AddedLastBeat);
		UE_LOG(LogTemp, Display, TEXT("Added Type 2 Attack to queue."));
	}
}

void APlayerCharacter::AddType3Attack()
{
	if (bInQTE) {
		QTE->AttemptAttack(Attacks::Attack_Type3);
	}
	else {
		bool AddedLastBeat = BeatManager->GetCurrentTimeSinceLastBeat() < BeatManager->AfterBeatGrace();
		ComboManager->AddAttack(Attacks::Attack_Type3, PlayerDamage, !AddedLastBeat);
		UE_LOG(LogTemp, Display, TEXT("Added Type 3 Attack to queue."));
	}
}

void APlayerCharacter::CameraShake()
{
	//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("Shake!"));

	//UGameplayStatics::PlayWorldCameraShake(this, <ULegacyCameraShake>(CameraShakeClass, )
	UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0)->StartCameraShake(UBBCameraShake::StaticClass());
}
void APlayerCharacter::SpawnParticle()
{
	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, TEXT("Particle!"));

	if (TempParticleEffect != NULL)
	{
		//UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), TempParticleEffect, GetActorLocation(), true);
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), TempParticleEffect, GetActorLocation(), FRotator(1), FVector(1), true, true, ENCPoolMethod::AutoRelease, true);
	}
}

void APlayerCharacter::AddNeutralBlock()
{
	if (!bIsBlocking) {
		CurrentBlockedType = Attacks::Attack_Neutral;
		bIsBlocking = true;
	}
}

void APlayerCharacter::AddType1Block()
{
	if (!bIsBlocking) {
		CurrentBlockedType = Attacks::Attack_Type1;
		bIsBlocking = true;
	}
}

void APlayerCharacter::AddType2Block()
{
	if (!bIsBlocking) {
		CurrentBlockedType = Attacks::Attack_Type2;
		bIsBlocking = true;
	}
}

void APlayerCharacter::AddType3Block()
{
	if (!bIsBlocking) {
		CurrentBlockedType = Attacks::Attack_Type3;
		bIsBlocking = true;
	}
}

void APlayerCharacter::AttackCallback(Attacks AttackType, float MotionValue, float AnimLength, int Combo, int ComboStep)
{
	//To Do: Add damage functionality...
	FString attackName;

	switch (AttackType) {
	case Attacks::Attack_Neutral:
		attackName = "Landed Neutral Attack";
		
		break;

	case Attacks::Attack_Type1:
		attackName = "Landed Type 1 Attack";
		
		break;

	case Attacks::Attack_Type2:
		attackName = "Landed Type 2 Attack";
		break;

	case Attacks::Attack_Type3:
		attackName = "Landed Type 3 Attack";
		break;

	case Attacks::Attack_Pause:
		attackName = "Paused";
		break;

	default:
		UE_LOG(LogTemp, Error, TEXT("Attack type not implemented!"));
	}

	UE_LOG(LogTemp, Warning, TEXT("%s on Combo step %i in Combo %i"), *attackName, ComboStep, Combo);
}

void APlayerCharacter::PlayAttackMontage(FName SectionName, bool AddTimeBetweenBeats)
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();

	if (AnimInstance != nullptr && AttackMontage != nullptr)
	{
		int32 Section = AttackMontage->GetSectionIndex(SectionName);

		float Length = AttackMontage->GetSectionLength(Section);

		TArray<const FAnimNotifyEvent*> Notifies;
		float Start, End;

		AttackMontage->GetSectionStartAndEndTime(Section, Start, End);

		AttackMontage->GetAnimNotifies(Start, End, false, Notifies);

		for (auto& Notify : Notifies) {
			if (Notify->NotifyName == TEXT("DisableBoxCollision")) {
				Length = Notify->GetTriggerTime() - Start;
				break;
			}
		}

		float PlayRate = 0;

		if (AddTimeBetweenBeats) {
			PlayRate = Length / (BeatManager->GetTimeUntilNextBeat() + BeatManager->TimeBetweenBeats());
		}
		else {
			PlayRate = Length / BeatManager->GetTimeUntilNextBeat();
		}

		AnimInstance->Montage_Play(AttackMontage, PlayRate);
		AnimInstance->Montage_JumpToSection(SectionName, AttackMontage);
	}
}

void APlayerCharacter::OnBeat(float CurrentTimeSinceLastBeat)
{
	FTimerHandle handle;

	GetWorldTimerManager().SetTimer(handle, this, &APlayerCharacter::ProcessIncomingAttacks, 0.01f, false);
}

void APlayerCharacter::ProcessIncomingAttacks()
{
	for (auto& [Enemy, EnemyType, Damage] : IncomingAttacks) {
		if (bIsBlocking && CurrentBlockedType == EnemyType) {
			float dot = GetActorForwardVector().Dot(Enemy->GetActorForwardVector());

			if (dot < 0) {
				Enemy->Parry();
			}
			else {
				ApplyDamage(Damage);
			}
		}
		else {
			ApplyDamage(Damage);
		}
		
	}

	IncomingAttacks.Empty();
	bIsBlocking = false;
}

void APlayerCharacter::EnterQTE()
{
	if (QTE) {
		QTE->StartQTE(&DebugQTEDescription);
		bInQTE = true;
	}
}

void APlayerCharacter::ExitQTE()
{
	if (QTE) {
		QTE->EndQTE();
		bInQTE = false;
	}
}

void APlayerCharacter::ApplyDamage(float Damage)
{
	CurrentHealth = FMath::Clamp(CurrentHealth - Damage, 0, MaxHealth);

	UE_LOG(LogTemp, Display, TEXT("Remaining health: %f"), CurrentHealth);

	if (ScoreManager) {
		ScoreManager->TookDamage();
	}

	if (!IsAlive()) {
		if (!bHasDied) {
			bHasDied = true;

			UE_LOG(LogTemp, Warning, TEXT("Player died!"));

			ABeatTheBeatsPlayerController* controller = Cast<ABeatTheBeatsPlayerController>(GetController());

			if (controller) {
				controller->GameOver();
				DisableInput(controller);
			}

			GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
			SetActorTickEnabled(false);

			BeatManager->UnBindFuncFromOnBeat(BeatHandle);
		}
	}
}

void APlayerCharacter::RotatePlayerToAttack(float DeltaTime)
{
	if (!bMovedThisTick && !bIsLockingTarget) {
		SetActorRotation(FMath::RInterpTo(GetActorRotation(), GetController()->GetControlRotation(), DeltaTime, RotationSpeed));
	}
}
