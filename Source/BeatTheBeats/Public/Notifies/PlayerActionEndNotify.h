// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "PlayerActionEndNotify.generated.h"

class APlayerCharacter;

/**
 * 
 */
UCLASS()
class BEATTHEBEATS_API UPlayerActionEndNotify : public UAnimNotify
{
	GENERATED_BODY()
	
public:

	UPlayerActionEndNotify(const FObjectInitializer& ObjectInitializer);

	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;

private:

	UPROPERTY()
	APlayerCharacter* Player = nullptr;
};
