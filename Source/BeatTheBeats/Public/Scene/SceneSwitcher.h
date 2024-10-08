// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SceneSwitcher.generated.h"

class UInputMappingContext;
class UInputAction;

UCLASS()
class BEATTHEBEATS_API ASceneSwitcher : public AActor
{
	GENERATED_BODY()
	
public:
    ASceneSwitcher();

    virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent);

    void KeyPressed();

protected:
    virtual void BeginPlay() override;

public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Level Switching")
    TArray<FName> LevelNames;

    // Function to switch to a level by index
    UFUNCTION(BlueprintCallable, Category = "Level Switching")
    void SwitchLevel();

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Level Switching")
    int32 CurrentScene = 0;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Level Switching")
    bool CanSwitchScene;

    UPROPERTY(EditAnywhere, Category = Input)
    UInputMappingContext* Mappings;

    UPROPERTY(EditAnywhere, Category = Input)
    UInputAction* KeyAction;


};