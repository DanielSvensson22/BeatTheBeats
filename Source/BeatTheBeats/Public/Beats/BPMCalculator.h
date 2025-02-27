#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "BPMCalculator.generated.h"

/**
 * 
 */
UCLASS()
class BEATTHEBEATS_API UBPMCalculator : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable) static float BPMDetector(USoundWave* SoundWave, float RMSMultiplier);
};
