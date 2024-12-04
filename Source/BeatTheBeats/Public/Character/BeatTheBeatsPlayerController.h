// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "BeatTheBeatsPlayerController.generated.h"

class UInputMappingContext;
class UUserWidget;
class ABeatManager;
class AScoreManager;

/**
 * 
 */
UCLASS()
class BEATTHEBEATS_API ABeatTheBeatsPlayerController : public APlayerController
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintPure)
	FORCEINLINE ABeatManager* GetBeatManager() { return BeatManager; }

	FORCEINLINE UUserWidget* GetHUD() { return HUD; }

	void GameOver();

	void ForceInit();
	
protected:

	/**
	* MappingContext 
	*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputMappingContext* DefaultMappingContext;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputMappingContext* ModifierMappingContext;

	virtual void BeginPlay() override;

private:

	UPROPERTY()
	UUserWidget* HUD;

	UPROPERTY(EditAnywhere)
	TSubclassOf<UUserWidget> HudClass;

	UPROPERTY(VisibleAnywhere)
	ABeatManager* BeatManager;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<ABeatManager> BeatManagerClass;

	UPROPERTY()
	UUserWidget* GameOverScreen;

	UPROPERTY(EditAnywhere)
	TSubclassOf<UUserWidget> GameOverScreenClass;

	AScoreManager* ScoreManager;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<AScoreManager> ScoreManagerClass;

	bool bHasBeenInitialized = false;
};
