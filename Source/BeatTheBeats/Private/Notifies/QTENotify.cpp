// Fill out your copyright notice in the Description page of Project Settings.


#include "Notifies/QTENotify.h"
#include "Character/PlayerCharacter.h"
#include "Weapons/WeaponBase.h"
#include "Kismet/GameplayStatics.h"

UQTENotify::UQTENotify(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	bShouldFireInEditor = false;
	NotifyColor = FColor::Green;
}

void UQTENotify::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) {
	Super::Notify(MeshComp, Animation, EventReference);

	FVector position = MeshComp->GetSocketLocation(Socket);
	FRotator rotation = MeshComp->GetSocketRotation(Socket);

	if (APlayerCharacter* player = Cast<APlayerCharacter>(UGameplayStatics::GetPlayerPawn(MeshComp, 0))) {
		if (player->GetWeapon()) {
			player->GetWeapon()->SpawnProjectile(Projectile, position, rotation);
		}
	}
}