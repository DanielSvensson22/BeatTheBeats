// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "TutorialChecklist.generated.h"

/**
 * 
 */
UENUM(BlueprintType)
enum class ETutorialStep : uint8
{
	Dodge UMETA(DisplayName = "Dodge"),
	Attack1 UMETA(DisplayName = "Attack1"),
	Attack2 UMETA(DisplayName = "Attack2"),
	Attack3 UMETA(DisplayName = "Attack3"),
	AttackNeutral UMETA(DisplayName = "AttackNeutral"),
	Block1 UMETA(DisplayName = "Block1"),
	Block2 UMETA(DisplayName = "Block2"),
	Block3 UMETA(DisplayName = "Block3"),
	BlockNeutral UMETA(DisplayName = "BlockNeutral"),
	Combo1230 UMETA(DisplayName = "Combo1230"),
	Combo1323 UMETA(DisplayName = "Combo1323")
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnTutorialStepCompleted, ETutorialStep, CompletedStep);

UCLASS()
class BEATTHEBEATS_API UTutorialChecklist : public UObject
{
	GENERATED_BODY()
	
public:
	UPROPERTY(BlueprintAssignable, Category = "Tutorial")
	FOnTutorialStepCompleted OnTutorialStepCompleted; // Broadcasting Tutorial Completions

	UPROPERTY(BlueprintReadWrite, Category = "Tutorial")
	TArray<bool> TutorialProgress;

	UTutorialChecklist()
	{
		// Initialize the array with default values (false)
		TutorialProgress.SetNum(static_cast<int32>(ETutorialStep::Combo1323) + 1); // Makes the Tutorial to fit all ETutorialSteps to Combo1323 (Enums value starts at 0 so just add a 1 to fit all the enums in the array)
	}
	UFUNCTION(BlueprintCallable, Category = "Tutorial")
	void MarkStepCompleted(ETutorialStep Step);
	UFUNCTION(BlueprintCallable, Category = "Tutorial")
	bool IsStepCompleted(ETutorialStep Step) const;
};
