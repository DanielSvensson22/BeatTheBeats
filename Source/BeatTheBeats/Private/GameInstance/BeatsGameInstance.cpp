// Fill out your copyright notice in the Description page of Project Settings.

#include "GameInstance/BeatsGameInstance.h"


void UBeatsGameInstance::Init()
{
    Super::Init();

    // Make Event that triggers when a new map is loaded
    FCoreUObjectDelegates::PostLoadMapWithWorld.AddUObject(this, &UBeatsGameInstance::HandlePostLoadMap);

    UWorld* World = GetWorld();

    SceneSwitcherInstance = World->SpawnActor<ASceneSwitcher>(SceneSwitcherClass);
}

void UBeatsGameInstance::HandlePostLoadMap(UWorld* LoadedWorld)
{
    if (LoadedWorld)
    {
        UE_LOG(LogTemp, Log, TEXT("A new level has been loaded. Spawning SceneSwitcher."));

        // Spawn the SceneSwitcher in the newly loaded world
        if (SceneSwitcherClass)
        {
            SceneSwitcherInstance = LoadedWorld->SpawnActor<ASceneSwitcher>(SceneSwitcherClass);
            SceneSwitcherInstance->CurrentScene = ++CurrentScene;
        }
    }
}