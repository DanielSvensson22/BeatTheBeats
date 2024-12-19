// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "../Combos/QTEDescription.h"
#include "../Combos/Combo.h"
#include "Styling/SlateColor.h"
#include "Math/Color.h"
#include "QTEComponent.generated.h"

class UImage;
class APlayerCharacter;
class UTextBlock;

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class BEATTHEBEATS_API UQTEComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UQTEComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void StartQTE(TArray<FQTEDescription>* qte, ComboEffect effect);

	void AttemptAttack(Attacks Attack);

	void EndQTE();

	void AddSpeed(float speed);

	FORCEINLINE void ResetTime() {
		CurrentTimeStep = 0;
	}

private:

	void UpdateWidgetIndicators(int index);

private:

	UUserWidget* Widget;

	float CurrentTimeStep = 0;

	int CurrentQTEStep = 0;

	TArray<FQTEDescription>* CurrentQTE;

	ComboEffect CurrentComboEffect;

	bool bIsActive = false;

	class ABeatManager* BeatManager;

	UPROPERTY(EditDefaultsOnly)
		TSubclassOf<ABeatManager> BeatManagerClass;

	APlayerCharacter* player;

	UImage* AttackIndicator;
	UImage* AttackCircle;
	UImage* MinClosenessIndicator;

	FVector2D StartPos;

	UPROPERTY(EditDefaultsOnly)
		FLinearColor NeutralColor;

	UPROPERTY(EditDefaultsOnly)
		FLinearColor Attack1Color;

	UPROPERTY(EditDefaultsOnly)
		FLinearColor Attack2Color;

	UPROPERTY(EditDefaultsOnly)
		FLinearColor Attack3Color;

	UPROPERTY(EditDefaultsOnly)
		float MaxCircleScale = 2;

	UPROPERTY(EditDefaultsOnly)
		float MinCircleScale = 0.5f;

	UPROPERTY(EditDefaultsOnly)
		float TimeStepMultiplier = 1;

	UPROPERTY(EditDefaultsOnly)
		float MinClosenessToBeat = 0.85f;

	float SpeedIncrease = 1;

	UTextBlock* FailText;
	UTextBlock* SuccessText;

	UPROPERTY(EditDefaultsOnly)
	float QTEStateTextLifeTime = 2;

	float QTEStateTextTime;
};