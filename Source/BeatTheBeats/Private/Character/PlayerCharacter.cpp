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
#include "Materials/MaterialInstanceDynamic.h"
#include "Interfaces/LockOnInterface.h"
#include "Components/AudioComponent.h"

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

	AudioComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("Sound Player"));
	AudioComponent->SetupAttachment(RootComponent);
}

APlayerCharacter::~APlayerCharacter()
{
	BeatManager->UnBindFuncFromOnBeat(BeatHandle);
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

		PlayAttackMontage(StartAnim, TEXT("Default"), BeatManager->TimeBetweenBeats() * 2);
	}

	ScoreManager = Cast<AScoreManager>(UGameplayStatics::GetActorOfClass(this, ScoreManagerClass));

	if (ScoreManager == nullptr) {
		UE_LOG(LogTemp, Error, TEXT("No Score Manager was found in the scene!"));
	}

	if (AttackTypeEffect) {
		AttackTypeEffectComp = UNiagaraFunctionLibrary::SpawnSystemAttached(AttackTypeEffect, GetMesh(), TEXT("pelvis"),
			FVector::ZeroVector, FRotator::ZeroRotator, EAttachLocation::KeepRelativeOffset, false);

		if (AttackTypeEffectComp) {
			UNiagaraFunctionLibrary::OverrideSystemUserVariableSkeletalMeshComponent(AttackTypeEffectComp, TEXT("Skeletal Mesh"), GetMesh());
			AttackTypeEffectComp->SetVariableFloat(TEXT("SpawnRate"), SpawnRate);
			AttackTypeEffectComp->SetVariableLinearColor(TEXT("Color"), NeutralColor);
		}
	}

	AttackTypeMaterial = GetMesh()->CreateDynamicMaterialInstance(AttackTypeMaterialIndex, GetMesh()->GetMaterial(AttackTypeMaterialIndex));

	if (AttackTypeMaterial) {
		AttackTypeMaterial->SetVectorParameterValue(LowColorName, LowNeutralColor);
		AttackTypeMaterial->SetVectorParameterValue(HighColorName, HighNeutralColor);

		GetMesh()->SetMaterial(AttackTypeMaterialIndex, AttackTypeMaterial);
	}

	SetNotifyName("Attack Window");
}

void APlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	RotatePlayerToAttack(DeltaTime);

	MovePlayerToAttack(DeltaTime);

	PerformDodge(DeltaTime);

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

		//Dodges
		EnhancedInputComponent->BindAction(DodgeBackAction, ETriggerEvent::Started, this, &APlayerCharacter::DodgeBack);
		EnhancedInputComponent->BindAction(DodgeLeftAction, ETriggerEvent::Started, this, &APlayerCharacter::DodgeLeft);
		EnhancedInputComponent->BindAction(DodgeRightAction, ETriggerEvent::Started, this, &APlayerCharacter::DodgeRight);
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

		if (CollisionEnable == ECollisionEnabled::NoCollision) {
			EnemyToAttack = nullptr;
			bClosingDistance = false;
			bIsAttacking = false;
			bInAttackAnimation = false;
			bIsBlocking = false;
			bIsDodging = false;

			if (bSpecial2Active) {
				bSpecial2Active = false;
				QTE->AddSpeed(Special2QTESpeedIncrease);
				QTE->StartQTE(&Special2QTE, ComboEffect::ExtraSpecial2);
				QTE->ResetTime();
				bInQTE = true;
			}
		}
		else {
			FHitResult result;
			FCollisionQueryParams params;
			params.AddIgnoredActor(this);
			params.AddIgnoredActor(Weapon);

			if (GetWorld()->LineTraceSingleByChannel(result, GetActorLocation(),
				GetActorLocation() + GetActorForwardVector() * MaxClosingDistance, ECollisionChannel::ECC_GameTraceChannel3, params)) {

				EnemyToAttack = Cast<AEnemyBase>(result.GetActor());

				UE_LOG(LogTemp, Warning, TEXT("Found enemy to close distance to"));

				if (EnemyToAttack) {
					float dist = FVector::Dist(GetActorLocation(), EnemyToAttack->GetActorLocation());

					if (dist > MinClosingDistance) {
						bClosingDistance = true;
						UE_LOG(LogTemp, Warning, TEXT("Closing Distance!"));
					}
					else {
						EnemyToAttack = nullptr;
					}
				}
			}
		}
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

	if (Controller != nullptr && !bIsAttacking && !bIsDodging)
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
		HitArray.Empty();
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

		TArray<FHitResult> SphereHitMulti; // hit result

		UKismetSystemLibrary::SphereTraceMultiForObjects(this, Start, End, TargetLockTraceRadius, ObjectTypesArray, false, IgnoreActors, EDrawDebugTrace::ForDuration, SphereHitMulti, true);

		int n = 0;

		for (const FHitResult& HitResult : SphereHitMulti)
		{
			ILockOnInterface* LockOnInterface = Cast<ILockOnInterface>(HitResult.GetActor());
			if (LockOnInterface != nullptr)
			{
				if (HitArray.Contains(HitResult.GetActor())) continue;
				HitArray.Add(HitResult.GetActor());
			}
		}

		for (AActor* Target : HitArray)
		{
			float Distance = GetDistanceTo(Target);
			if (Distance <= ClosestDistance)
			{
				ClosestDistance = Distance;
				TargetLockHitTarget = Target;
				bIsLockingTarget = true;
			}
		}
		n = HitArray.Num();
		UE_LOG(LogTemp, Warning, TEXT("%d"), n)
	}
}

void APlayerCharacter::SetTargetLockCamera()
{
	if (TargetLockHitTarget != nullptr)
	{
		ClosestDistance = 100000.f;
		float Distance = GetDistanceTo(TargetLockHitTarget);
		FVector LockOffset = TargetLockHitTarget->GetActorUpVector() * Distance * LockOffsetModifier;

		GetController()->SetControlRotation(UKismetMathLibrary::FindLookAtRotation(GetActorLocation() + LockOffset, TargetLockHitTarget->GetActorLocation()));

		if (Distance > TargetLockMaxMoveDistance)
		{
			TargetLockHitTarget = nullptr;
			bIsLockingTarget = false;
			bUseControllerRotationYaw = false;
			GetCharacterMovement()->bOrientRotationToMovement = true;
			ECameraState::ECS_FreeCamera;
			HitArray.Empty();
		}
	}
}

void APlayerCharacter::AddNeutralAttack()
{
	if (bInQTE) {
		QTE->AttemptAttack(Attacks::Attack_Neutral);
	}
	else {
		if (!bIsDodging && !bIsBlocking) {
			bool AddedLastBeat = BeatManager->GetCurrentTimeSinceLastBeat() < BeatManager->AfterBeatGrace();
			ComboManager->AddAttack(Attacks::Attack_Neutral, PlayerDamage, !AddedLastBeat);
			bIsAttacking = true;

			if (AttackTypeEffectComp) {
				AttackTypeEffectComp->SetVariableLinearColor(TEXT("Color"), NeutralColor);
			}

			if (AttackTypeMaterial) {
				AttackTypeMaterial->SetVectorParameterValue(LowColorName, LowNeutralColor);
				AttackTypeMaterial->SetVectorParameterValue(HighColorName, HighNeutralColor);

				GetMesh()->SetMaterial(AttackTypeMaterialIndex, AttackTypeMaterial);
			}
		}
	}
}

void APlayerCharacter::AddType1Attack()
{
	if (bInQTE) {
		QTE->AttemptAttack(Attacks::Attack_Type1);
	}
	else {
		if (!bIsDodging && !bIsBlocking) {
			bool AddedLastBeat = BeatManager->GetCurrentTimeSinceLastBeat() < BeatManager->AfterBeatGrace();
			ComboManager->AddAttack(Attacks::Attack_Type1, PlayerDamage, !AddedLastBeat);
			bIsAttacking = true;

			if (AttackTypeEffectComp) {
				AttackTypeEffectComp->SetVariableLinearColor(TEXT("Color"), AttackOneColor);
			}

			if (AttackTypeMaterial) {
				AttackTypeMaterial->SetVectorParameterValue(LowColorName, LowAttack1Color);
				AttackTypeMaterial->SetVectorParameterValue(HighColorName, HighAttack1Color);

				GetMesh()->SetMaterial(AttackTypeMaterialIndex, AttackTypeMaterial);
			}
		}
	}
}

void APlayerCharacter::AddType2Attack()
{
	if (bInQTE) {
		QTE->AttemptAttack(Attacks::Attack_Type2);
	}
	else {
		if (!bIsDodging && !bIsBlocking) {
			bool AddedLastBeat = BeatManager->GetCurrentTimeSinceLastBeat() < BeatManager->AfterBeatGrace();
			ComboManager->AddAttack(Attacks::Attack_Type2, PlayerDamage, !AddedLastBeat);
			bIsAttacking = true;

			if (AttackTypeEffectComp) {
				AttackTypeEffectComp->SetVariableLinearColor(TEXT("Color"), AttackTwoColor);
			}

			if (AttackTypeMaterial) {
				AttackTypeMaterial->SetVectorParameterValue(LowColorName, LowAttack2Color);
				AttackTypeMaterial->SetVectorParameterValue(HighColorName, HighAttack2Color);

				GetMesh()->SetMaterial(AttackTypeMaterialIndex, AttackTypeMaterial);
			}
		}
	}
}

void APlayerCharacter::AddType3Attack()
{
	if (bInQTE) {
		QTE->AttemptAttack(Attacks::Attack_Type3);
	}
	else {
		if (!bIsDodging && !bIsBlocking) {
			bool AddedLastBeat = BeatManager->GetCurrentTimeSinceLastBeat() < BeatManager->AfterBeatGrace();
			ComboManager->AddAttack(Attacks::Attack_Type3, PlayerDamage, !AddedLastBeat);
			bIsAttacking = true;

			if (AttackTypeEffectComp) {
				AttackTypeEffectComp->SetVariableLinearColor(TEXT("Color"), AttackThreeColor);
			}

			if (AttackTypeMaterial) {
				AttackTypeMaterial->SetVectorParameterValue(LowColorName, LowAttack3Color);
				AttackTypeMaterial->SetVectorParameterValue(HighColorName, HighAttack3Color);

				GetMesh()->SetMaterial(AttackTypeMaterialIndex, AttackTypeMaterial);
			}
		}
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
	if (!bIsBlocking && !bIsDodging) {
		CurrentBlockedType = Attacks::Attack_Neutral;
		bIsBlocking = true;

		if (BeatManager->ClosenessToBeat() > ClosenessForPeffectBlock) {
			bPerfectBlock = true;
		}
		else {
			bPerfectBlock = false;
		}

		PlayAttackMontage(BlockAnim, TEXT("Default"), std::max(BeatManager->GetTimeUntilNextBeat(), BeatManager->TimeBetweenBeats() / 2));
	}
}

void APlayerCharacter::AddType1Block()
{
	if (!bIsBlocking && !bIsDodging) {
		CurrentBlockedType = Attacks::Attack_Type1;
		bIsBlocking = true;

		if (BeatManager->ClosenessToBeat() > ClosenessForPeffectBlock) {
			bPerfectBlock = true;
		}
		else {
			bPerfectBlock = false;
		}

		PlayAttackMontage(BlockAnim, TEXT("Default"), std::max(BeatManager->GetTimeUntilNextBeat(), BeatManager->TimeBetweenBeats() / 2));
	}
}

void APlayerCharacter::AddType2Block()
{
	if (!bIsBlocking && !bIsDodging) {
		CurrentBlockedType = Attacks::Attack_Type2;
		bIsBlocking = true;

		if (BeatManager->ClosenessToBeat() > ClosenessForPeffectBlock) {
			bPerfectBlock = true;
		}
		else {
			bPerfectBlock = false;
		}

		PlayAttackMontage(BlockAnim, TEXT("Default"), std::max(BeatManager->GetTimeUntilNextBeat(), BeatManager->TimeBetweenBeats() / 2));
	}
}

void APlayerCharacter::AddType3Block()
{
	if (!bIsBlocking && !bIsDodging) {
		CurrentBlockedType = Attacks::Attack_Type3;
		bIsBlocking = true;

		if (BeatManager->ClosenessToBeat() > ClosenessForPeffectBlock) {
			bPerfectBlock = true;
		}
		else {
			bPerfectBlock = false;
		}

		PlayAttackMontage(BlockAnim, TEXT("Default"), std::max(BeatManager->GetTimeUntilNextBeat(), BeatManager->TimeBetweenBeats() / 2));
	}
}

void APlayerCharacter::DodgeBack()
{
	if (!bIsDodging) {
		bIsDodging = true;
		TimeUntilInvincibilityEnds = InvincibilityDuration;

		FVector offset = GetActorForwardVector() * -1 * DodgeDistance;

		DodgeLocation = GetActorLocation() + offset;

		PlayAttackMontage(DodgeBackAnim, TEXT("Default"), std::max(BeatManager->GetTimeUntilNextBeat(), BeatManager->TimeBetweenBeats() / 2));
	}
}

void APlayerCharacter::DodgeLeft()
{
	if (!bIsDodging) {
		bIsDodging = true;
		TimeUntilInvincibilityEnds = InvincibilityDuration;

		FVector offset = GetActorRightVector() * -1 * DodgeDistance;

		DodgeLocation = GetActorLocation() + offset;

		PlayAttackMontage(DodgeLeftAnim, TEXT("Default"), std::max(BeatManager->GetTimeUntilNextBeat(), BeatManager->TimeBetweenBeats() / 2));
	}
}

void APlayerCharacter::DodgeRight()
{
	if (!bIsDodging) {
		bIsDodging = true;
		TimeUntilInvincibilityEnds = InvincibilityDuration;

		FVector offset = GetActorRightVector() * DodgeDistance;

		DodgeLocation = GetActorLocation() + offset;

		PlayAttackMontage(DodgeRightAnim, TEXT("Default"), std::max(BeatManager->GetTimeUntilNextBeat(), BeatManager->TimeBetweenBeats() / 2));
	}
}

void APlayerCharacter::AttackCallback(TArray<FQTEDescription>* qte, ComboEffect effect)
{
	CurrentQTEDescription = qte;
	CurrentComboEffect = effect;

	FTimerHandle handle;

	GetWorldTimerManager().SetTimer(handle, this, &APlayerCharacter::EnterQTE, BeatManager->TimeBetweenBeats(), false);
}

void APlayerCharacter::PlayAttackMontage(UAnimMontage* montage, FName SectionName, float TotalTime)
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();

	if (AnimInstance != nullptr && montage != nullptr)
	{
		bInAttackAnimation = true;

		int32 Section = montage->GetSectionIndex(SectionName);

		float Length = montage->GetSectionLength(Section);

		TArray<const FAnimNotifyEvent*> Notifies;
		float Start, End;

		montage->GetSectionStartAndEndTime(Section, Start, End);

		montage->GetAnimNotifies(Start, End, false, Notifies);


		for (auto& Notify : Notifies) {
			if (Notify->NotifyName == AttackNotifyName) {
				Length = Notify->GetEndTriggerTime() - Start;
				break;
			}
		}

		float PlayRate = Length / TotalTime;

		AnimInstance->Montage_Play(montage, PlayRate);
		AnimInstance->Montage_JumpToSection(SectionName, montage);
	}
}

void APlayerCharacter::OnBeat(float CurrentTimeSinceLastBeat)
{
	FTimerHandle handle;

	GetWorldTimerManager().SetTimer(handle, this, &APlayerCharacter::ProcessIncomingAttacks, 0.01f, false);
}

void APlayerCharacter::ProcessIncomingAttacks()
{
	if (bIsDodging) {
		IncomingAttacks.Empty();
		bIsBlocking = false;
		bIsDodging = false;
		return;
	}

	for (auto& [Enemy, EnemyType, Damage] : IncomingAttacks) {
		if (bIsBlocking && CurrentBlockedType == EnemyType) {
			float dot = GetActorForwardVector().Dot(Enemy->GetActorForwardVector());

			if (dot < 0) {
				Enemy->Parry();

				if (BlockSound) {
					AudioComponent->SetSound(BlockSound);
					AudioComponent->Play();
				}

				UNiagaraComponent* NiagaraComp = UNiagaraFunctionLibrary::SpawnSystemAttached(BlockEffect, GetMesh(), TEXT("shield_outer"),
					FVector::ZeroVector, FRotator::ZeroRotator, EAttachLocation::KeepRelativeOffset, true);

				if (!bPerfectBlock) {
					ApplyDamage(Damage / 2);
					NiagaraComp->SetVariableLinearColor(TEXT("Color"), FLinearColor::Blue);
				}
				else {
					NiagaraComp->SetVariableLinearColor(TEXT("Color"), FLinearColor::Red);
				}
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
		QTE->StartQTE(CurrentQTEDescription, CurrentComboEffect);
		bInQTE = true;
	}
}

void APlayerCharacter::ExitQTE()
{
	if (QTE) {
		CurrentQTEDescription = nullptr;
		bInQTE = false;
	}
}

void APlayerCharacter::FailedSpecial()
{
	Weapon->SetAttackStatus(FailedSpecialDamage, Attacks::Attack_Guaranteed, true);
	PlayAttackMontage(FailedSpecialAnim, TEXT("Default"), BeatManager->GetTimeUntilNextBeat() + BeatManager->TimeBetweenBeats());
	bIsDodging = true;
}

void APlayerCharacter::Special1()
{
	Weapon->SetAttackStatus(Special1Damage, Attacks::Attack_Guaranteed, true);
	PlayAttackMontage(Special1Anim, TEXT("Default"), BeatManager->GetTimeUntilNextBeat() + BeatManager->TimeBetweenBeats() * 3);
	bIsDodging = true;
}

void APlayerCharacter::Special2()
{
	Weapon->SetAttackStatus(Special2Damage, Attacks::Attack_Guaranteed, true);
	PlayAttackMontage(Special2Anim, TEXT("Default"), BeatManager->GetTimeUntilNextBeat() + BeatManager->TimeBetweenBeats() * 2);
	bIsDodging = true;
	bSpecial2Active = true;
}

void APlayerCharacter::Special3()
{
	Weapon->SetAttackStatus(Special3Damage, Attacks::Attack_Guaranteed, true);
	PlayAttackMontage(Special3Anim, TEXT("Default"), BeatManager->GetTimeUntilNextBeat() + BeatManager->TimeBetweenBeats());
	bIsDodging = true;
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

			if (AttackTypeEffectComp) {
				AttackTypeEffectComp->Deactivate();
			}

			BeatManager->UnBindFuncFromOnBeat(BeatHandle);

			controller->bShowMouseCursor = true;
			controller->bEnableClickEvents = true;
			controller->bEnableMouseOverEvents = true;

			PlayAttackMontage(DeathAnim, TEXT("Default"), BeatManager->TimeBetweenBeats() + BeatManager->GetTimeUntilNextBeat());

			if (DeathSound) {
				AudioComponent->SetSound(DeathSound);
				AudioComponent->Play();
			}
		}
	}
	else {
		if (HitSound && (!AudioComponent->IsPlaying() || AudioComponent->GetSound() != BlockSound)) {
			AudioComponent->SetSound(HitSound);
			AudioComponent->Play();
		}
	}

	UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0)->StartCameraShake(UBBCameraShake::StaticClass());
}

void APlayerCharacter::RotatePlayerToAttack(float DeltaTime)
{
	if (!bMovedThisTick && !bIsLockingTarget && !bIsDodging) {
		SetActorRotation(FMath::RInterpTo(GetActorRotation(), GetController()->GetControlRotation(), DeltaTime, RotationSpeed));
	}
}

void APlayerCharacter::MovePlayerToAttack(float DeltaTime)
{
	if (bClosingDistance && EnemyToAttack != nullptr) {
		FVector offset = GetActorLocation() - EnemyToAttack->GetActorLocation();
		offset.Normalize(1);
		offset *= EnemyToAttack->GetCapsuleComponent()->GetScaledCapsuleRadius() * 2;
		SetActorLocation(FMath::VInterpConstantTo(GetActorLocation(), EnemyToAttack->GetActorLocation() + offset, DeltaTime, ClosingDistanceSpeed));
	}
}

void APlayerCharacter::PerformDodge(float DeltaTime)
{
	if (bIsDodging) {
		TimeUntilInvincibilityEnds -= DeltaTime;

		if (TimeUntilInvincibilityEnds <= 0) {
			bIsDodging = false;
		}

		FHitResult result;
		SetActorLocation(FMath::VInterpConstantTo(GetActorLocation(), DodgeLocation, DeltaTime, DodgeSpeed), true, &result, ETeleportType::TeleportPhysics);
	}
}

void APlayerCharacter::ReloadLevel_Implementation()
{

}