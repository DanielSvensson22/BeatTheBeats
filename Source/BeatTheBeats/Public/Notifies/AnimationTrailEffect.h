// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AnimNotifyState_TimedNiagaraEffect.h"
#include "AnimationTrailEffect.generated.h"

/**
 * 
 */
UCLASS()
class BEATTHEBEATS_API UAnimationTrailEffect : public UAnimNotifyState_TimedNiagaraEffect
{
	GENERATED_BODY()
	
public:

	UAnimationTrailEffect(const FObjectInitializer& ObjectInitializer);

	virtual void NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime, const FAnimNotifyEventReference& EventReference) override;

private:

	UPROPERTY(EditAnywhere)
	FName StartSocket;

	UPROPERTY(EditAnywhere)
	FName EndSocket;
};
