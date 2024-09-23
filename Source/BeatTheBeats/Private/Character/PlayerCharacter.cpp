// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/PlayerCharacter.h"
#include "GameFramework\SpringArmComponent.h"
#include "GameFramework\CharacterMovementComponent.h"
#include "Camera/CameraComponent.h"
#include "ComboManagerComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"

#include "Components/InputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"

APlayerCharacter::APlayerCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.f, 600.f, 0.f);

	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(GetRootComponent());
	CameraBoom->AddRelativeLocation(FVector(0.f, 0.f, 50.f));
	CameraBoom->TargetArmLength = 350.f;
	CameraBoom->bEnableCameraRotationLag = true;
	CameraBoom->CameraRotationLagSpeed = 20.f;

	ViewCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("ViewCamera"));
	ViewCamera->SetupAttachment(CameraBoom);

	ComboManager = CreateDefaultSubobject<UComboManagerComponent>(TEXT("Combo Manager"));
	ComboManager->BindAttackCallbackFunc(this, &APlayerCharacter::AttackCallback);

	TargetLockTraceRange = 1000.f;
	TargetLockTraceRadius = 150.f;
	MaxDistanceBetweenLockedTarget = 1500.f;
}

void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

void APlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	SetTargetLockCamera();

}

void APlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
	{
		EnhancedInputComponent->BindAction(MovementAction, ETriggerEvent::Triggered, this, &APlayerCharacter::Move);
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &APlayerCharacter::Look);
		EnhancedInputComponent->BindAction(TargetLockAction, ETriggerEvent::Started, this, &APlayerCharacter::TargetLock);
		EnhancedInputComponent->BindAction(NeautralAttackAction, ETriggerEvent::Started, this, &APlayerCharacter::AddNeutralAttack);
		EnhancedInputComponent->BindAction(Type1AttackAction, ETriggerEvent::Started, this, &APlayerCharacter::AddType1Attack);
		EnhancedInputComponent->BindAction(Type2AttackAction, ETriggerEvent::Started, this, &APlayerCharacter::AddType2Attack);
		EnhancedInputComponent->BindAction(Type3AttackAction, ETriggerEvent::Started, this, &APlayerCharacter::AddType3Attack);
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
		ObjectTypesArray.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_Pawn));

		TArray<AActor*> IgnoreActors; // leave blank if not needed

		// IgnoreActors.Add(); // Add actors to ingore here if needed

		FHitResult SphereHit; // hit result

		UKismetSystemLibrary::SphereTraceSingleForObjects(this, Start, End, TargetLockTraceRadius, ObjectTypesArray, false, IgnoreActors, EDrawDebugTrace::ForDuration, SphereHit, true);

		TargetLockHitTarget = SphereHit.GetActor();

		if (TargetLockHitTarget != nullptr)
		{
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

		if (Distance > 1500.f)
		{
			TargetLockHitTarget = nullptr;
			bIsLockingTarget = false;
			bUseControllerRotationYaw = false;
		}
	}
}

void APlayerCharacter::AddNeutralAttack()
{
	//To Do: Calculate damage based on performance...
	ComboManager->AddAttack(Attacks::Attack_Neutral, 1);
	UE_LOG(LogTemp, Display, TEXT("Added Neutral Attack to queue."));
}

void APlayerCharacter::AddType1Attack()
{
	//To Do: Calculate damage based on performance...
	ComboManager->AddAttack(Attacks::Attack_Type1, 1);
	UE_LOG(LogTemp, Display, TEXT("Added Type 1 Attack to queue."));
}

void APlayerCharacter::AddType2Attack()
{
	//To Do: Calculate damage based on performance...
	ComboManager->AddAttack(Attacks::Attack_Type2, 1);
	UE_LOG(LogTemp, Display, TEXT("Added Type 2 Attack to queue."));
}

void APlayerCharacter::AddType3Attack()
{
	//To Do: Calculate damage based on performance...
	ComboManager->AddAttack(Attacks::Attack_Type3, 1);
	UE_LOG(LogTemp, Display, TEXT("Added Type 3 Attack to queue."));
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
