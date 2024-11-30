// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "../Combos/QTEProjectile.h"
#include "QTENotify.generated.h"

/**
 * 
 */
UCLASS()
class BEATTHEBEATS_API UQTENotify : public UAnimNotify
{
	GENERATED_BODY()
	
public:

	UQTENotify(const FObjectInitializer& ObjectInitializer);

	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;

private:

	UPROPERTY(EditAnywhere)
	FName Socket;

	UPROPERTY(EditAnywhere)
	TSubclassOf<AQTEProjectile> Projectile;
};
