// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Math/UnrealMathUtility.h"
#include "BeatManager.generated.h"

DECLARE_MULTICAST_DELEGATE_OneParam(OnBeatDelegate, float);

UCLASS()
class BEATTHEBEATS_API ABeatManager : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABeatManager();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintPure)
	FORCEINLINE float GetCurrentTimeSinceLastBeat() const { return CurrentTimeSinceLastBeat; }

	UFUNCTION(BlueprintPure)
	FORCEINLINE float TimeBetweenBeats() const { return 60 / BPM; }

	UFUNCTION(BlueprintPure)
	FORCEINLINE float GetTimeUntilNextBeat() const { return TimeBetweenBeats() - CurrentTimeSinceLastBeat; }

	UFUNCTION(BlueprintPure)
	FORCEINLINE float AfterBeatGrace() const { return TimeBetweenBeats() / AfterBeatGraceDivider; }

	UFUNCTION(BlueprintPure)
	FORCEINLINE float GetIndicatorScale(float MaxIndicatorScale, float MinIndicatorScale) const {
		return FMath::InterpSinIn(MaxIndicatorScale, MinIndicatorScale, CurrentTimeSinceLastBeat / TimeBetweenBeats());
	}

	FORCEINLINE float GetIndicatorScale(float MaxIndicatorScale, float MinIndicatorScale, float Step) const {
		return FMath::InterpSinIn(MaxIndicatorScale, MinIndicatorScale, Step / TimeBetweenBeats());
	}

	UFUNCTION(BlueprintPure)
	FORCEINLINE float ClosenessToBeat() const {
		if (CurrentTimeSinceLastBeat < AfterBeatGrace()) {
			return std::abs(TimeBetweenBeats() - CurrentTimeSinceLastBeat) / TimeBetweenBeats();
		}
		else {
			return CurrentTimeSinceLastBeat / TimeBetweenBeats();
		}
	}

	UFUNCTION(BlueprintCallable)
	FORCEINLINE void SetBPM(float bpm) {
		BPM = bpm;
	}

	template<typename Type>
	FDelegateHandle BindFuncToOnBeat(Type* type, void (Type::* Func)(float)) {
		return OnBeat.AddUObject(type, Func);
	}

	void UnBindFuncFromOnBeat(FDelegateHandle handle) {
		if (OnBeat.Remove(handle)) {

		}
		else {
			//UE_LOG(LogTemp, Error, TEXT("Could not remove function from Beat Manager!"));
		}
	}

	UFUNCTION(BlueprintCallable)
	FORCEINLINE void ResetBeat() {
		CurrentTimeSinceLastBeat = 0;
	}

private:

	UPROPERTY(EditAnywhere)
	float BPM = 60;

	UPROPERTY(VisibleAnywhere)
	float CurrentTimeSinceLastBeat = 0;

	UPROPERTY(EditDefaultsOnly)
	float AfterBeatGraceDivider = 10.0f;

	OnBeatDelegate OnBeat;

};
