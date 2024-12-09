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
    UFUNCTION(BlueprintPure)
    FORCEINLINE float GetMusicVolume() const { return MusicVolume; }
    UFUNCTION(BlueprintPure)
    FORCEINLINE float GetEffectsVolume() const { return EffectsVolume; }
    UFUNCTION(BlueprintPure)
    FORCEINLINE bool GetCameraShake() const { return CameraShake; }

    UFUNCTION(BlueprintCallable)
    FORCEINLINE void SetMusicVolume(float newMusic) { MusicVolume = newMusic; }
    UFUNCTION(BlueprintCallable)
    FORCEINLINE void SetEffectsVolume(float newEffects) { EffectsVolume = newEffects; }
    UFUNCTION(BlueprintCallable)
    FORCEINLINE void SetCameraShake(bool newShake) { CameraShake = newShake; }

private:
    UPROPERTY(VisibleAnywhere)
    float MusicVolume;
    UPROPERTY(VisibleAnywhere)
    float EffectsVolume;
    UPROPERTY(VisibleAnywhere)
    bool CameraShake;
};
