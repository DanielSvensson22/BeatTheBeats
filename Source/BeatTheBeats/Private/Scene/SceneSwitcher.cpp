// Fill out your copyright notice in the Description page of Project Settings.


#include "Scene/SceneSwitcher.h"
#include "Kismet/GameplayStatics.h"
#include "GameInstance/BeatsGameInstance.h"

#pragma region Input
#include "Components/InputComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#pragma endregion

ASceneSwitcher::ASceneSwitcher()
{
	PrimaryActorTick.bCanEverTick = false;
	CanSwitchScene = true;
}

void ASceneSwitcher::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
	{
		EnhancedInputComponent->BindAction(KeyAction, ETriggerEvent::Triggered, this, &ASceneSwitcher::KeyPressed);
	}
}

void ASceneSwitcher::KeyPressed()
{
	if (!CanSwitchScene) return;
	SwitchLevel();
	CanSwitchScene = false;
}


void ASceneSwitcher::BeginPlay()
{
	Super::BeginPlay();
	// Get a reference to the GameInstance
	UBeatsGameInstance* GameInstance = Cast<UBeatsGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));

	if (GameInstance)
	{
		// Store the SceneSwitcher reference in the GameInstance
		GameInstance->SceneSwitcherInstance = this;
	}

	if (APlayerController* PlayerController = Cast<APlayerController>(GetWorld()->GetFirstPlayerController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(Mappings, 0);
			SetupPlayerInputComponent(PlayerController->InputComponent);
		}
	}

}

void ASceneSwitcher::SwitchLevel()
{
	int32 NewLevel = CurrentScene % LevelNames.Num();
	FName SelectedLevel = LevelNames[FMath::Abs(NewLevel)];
	UGameplayStatics::OpenLevel(GetWorld(), SelectedLevel);
}
