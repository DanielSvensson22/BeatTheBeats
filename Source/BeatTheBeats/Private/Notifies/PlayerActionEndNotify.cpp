// Fill out your copyright notice in the Description page of Project Settings.


#include "Notifies/PlayerActionEndNotify.h"
#include "Kismet/GameplayStatics.h"
#include "Character/PlayerCharacter.h"

UPlayerActionEndNotify::UPlayerActionEndNotify(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
#if WITH_EDITOR
	bShouldFireInEditor = false;
	NotifyColor = FColor::Yellow;
#endif
}

void UPlayerActionEndNotify::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);

	if (Player == nullptr) {
		APawn* pawn = UGameplayStatics::GetPlayerPawn(MeshComp, 0);
		Player = Cast<APlayerCharacter>(pawn);
	}

	if (Player != nullptr) {
		Player->SetWeaponCollisionEnabled(ECollisionEnabled::NoCollision);
	}
}
