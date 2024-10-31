// Fill out your copyright notice in the Description page of Project Settings.


#include "Notifies/AnimationTrailEffect.h"
#include "Particles/ParticleSystemComponent.h"

UAnimationTrailEffect::UAnimationTrailEffect(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	NotifyColor = FColor::Cyan;
	bShouldFireInEditor = true;

	SocketName = TEXT("root");
}

void UAnimationTrailEffect::NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyTick(MeshComp, Animation, FrameDeltaTime, EventReference);

	UFXSystemComponent* FXSystem = GetSpawnedEffect(MeshComp);

	if (FXSystem != nullptr) {
		FXSystem->SetVectorParameter(TEXT("StartSocket"), MeshComp->GetSocketLocation(StartSocket));
		FXSystem->SetVectorParameter(TEXT("EndSocket"), MeshComp->GetSocketLocation(EndSocket));
	}
}
