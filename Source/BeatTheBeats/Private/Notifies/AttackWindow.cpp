// Fill out your copyright notice in the Description page of Project Settings.


#include "Notifies/AttackWindow.h"
#include "Kismet/GameplayStatics.h"
#include "Character/PlayerCharacter.h"

UAttackWindow::UAttackWindow(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	NotifyColor = FColor::Red;
	bShouldFireInEditor = false;
	bIsNativeBranchingPoint = true;
}

void UAttackWindow::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration);

	if (Player == nullptr) {
		APawn* pawn = UGameplayStatics::GetPlayerPawn(MeshComp, 0);
		Player = Cast<APlayerCharacter>(pawn);
	}

	if (Player != nullptr) {
		Player->SetWeaponCollisionEnabled(ECollisionEnabled::QueryOnly);
	}
}

void UAttackWindow::NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyTick(MeshComp, Animation, FrameDeltaTime);
	//Add code if needed.
}

void UAttackWindow::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyEnd(MeshComp, Animation);

	if (Player != nullptr) {
		Player->SetWeaponCollisionEnabled(ECollisionEnabled::NoCollision);
	}
}
