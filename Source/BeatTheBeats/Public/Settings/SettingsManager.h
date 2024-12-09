// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SettingsManager.generated.h"

UCLASS()
class BEATTHEBEATS_API ASettingsManager : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ASettingsManager();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintPure)
	FORCEINLINE float GetMusicVolume() const { return MusicVolume; }
	UFUNCTION(BlueprintPure)
	FORCEINLINE float GetEffectsVolume() const { return EffectsVolume; }
	UFUNCTION(BlueprintPure)
	FORCEINLINE bool GetCameraShake() const { return CameraShake; }

	UFUNCTION(BlueprintCallable)
	void SetMusicVolume(float newMusic);
	UFUNCTION(BlueprintCallable)
	void SetEffectsVolume(float newEffects);
	UFUNCTION(BlueprintCallable)
	void SetCameraShake(bool newShake);
	UFUNCTION(BlueprintCallable)
	void UpdateSettings();
	UFUNCTION(BlueprintCallable)
	void SaveToFile();

	//Get the file path for the .json save file
	void GetSaveFilePath();

private:
	void ReadFromFile();

	UPROPERTY(VisibleAnywhere)
	float MusicVolume;
	UPROPERTY(VisibleAnywhere)
	float EffectsVolume;
	UPROPERTY(VisibleAnywhere)
	bool CameraShake;
	FString FilePath;
	FString Settings;
};