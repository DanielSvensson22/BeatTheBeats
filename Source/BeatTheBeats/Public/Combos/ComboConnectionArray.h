// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ComboConnection.h"
#include "ComboConnectionArray.generated.h"

/**
 * 
 */
USTRUCT(BlueprintType)
struct BEATTHEBEATS_API FComboConnectionArray
{
	GENERATED_BODY()

public:
	FComboConnectionArray();
	~FComboConnectionArray();

	UPROPERTY(EditDefaultsOnly)
	TArray<FComboConnection> Connections;
};
