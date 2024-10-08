// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "Scene/SceneSwitcher.h"
#include "BeatsGameInstance.generated.h"

/**
 * 
 */
UCLASS()
class BEATTHEBEATS_API UBeatsGameInstance : public UGameInstance
{
	GENERATED_BODY()
	
public:
    // Initialize the GameInstance
    virtual void Init() override;

    // Function to spawn in a new SceneSwitcher after level load
    void HandlePostLoadMap(UWorld* LoadedWorld);

public:
    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Level Switching")
    ASceneSwitcher* SceneSwitcherInstance;

    int32 CurrentScene;

    // SceneSwitcher to spawn in new levels
    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Level Switching")
    TSubclassOf<ASceneSwitcher> SceneSwitcherClass;
};
