// Fill out your copyright notice in the Description page of Project Settings.


#include "Combos/ComboManagerComponent.h"
#include "Beats/BeatManager.h"
#include "Kismet/GameplayStatics.h"
#include "Combos/Combo.h"
#include <tuple>
#include <queue>
#include "Character/PlayerCharacter.h"

// Sets default values for this component's properties
UComboManagerComponent::UComboManagerComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...
}


// Called when the game starts
void UComboManagerComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
	BeatManager = Cast<ABeatManager>(UGameplayStatics::GetActorOfClass(this, BeatManagerClass));

	if (BeatManager == nullptr) {
		UE_LOG(LogTemp, Error, TEXT("No Beat Manager was found in the scene!"));
	}
	else {
		BeatManager->BindFuncToOnBeat(this, &UComboManagerComponent::ProcessNextAttack);
	}

	if (Combos.Num() != ComboObtainedStatus.Num()) {
		UE_LOG(LogTemp, Error, TEXT("Combo count not equal to combo obtained status count!"));
	}

	if (ComboConnectionsList.Num() != Combos.Num()) {
		UE_LOG(LogTemp, Error, TEXT("Not every combo has a combo connections list!"));
	}
}


// Called every frame
void UComboManagerComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UComboManagerComponent::AddAttack(Attacks AttackType, float Damage, bool PlayerAddedThisBeat)
{
	if (StoredAttacks.size() < 2) {
		StoredAttacks.emplace(AttackType, Damage, PlayerAddedThisBeat);

		if (player) {
			if (StoredAttacks.size() == 1) {
				PerformAnimation(AttackType, BeatManager->ClosenessToBeat(), PlayerAddedThisBeat);
			}
			else {
				if (UpcomingAttackAnims.size() > 0) {
					UpcomingAttackAnims.emplace(AttackType, BeatManager->ClosenessToBeat(), true);
				}
				else {
					auto& [pad1, pad2, ShouldNotAttack] = StoredAttacks.front();
					UpcomingAttackAnims.emplace(AttackType, BeatManager->ClosenessToBeat(), !ShouldNotAttack);
				}
			}
		}
	}
}

void UComboManagerComponent::ProcessNextAttack(float CurrentTimeSinceLastBeat)
{
	if (StoredAttacks.size() > 0) {
		StoredAttack& nextAttack = StoredAttacks.front();
		auto& [AttackType, Damage, AddedOnThisBeat] = nextAttack;

		if (AddedOnThisBeat) {
			nextAttack = std::make_tuple(AttackType, Damage, false);
		}
		else {
			StoredAttacks.pop();

			if (StoredAttacks.size() > 0) {
				StoredAttack& nextNextAttack = StoredAttacks.front();
				auto& [AttackTypeNext, DamageNext, AddedOnThisBeatNext] = nextNextAttack;
				nextNextAttack = std::make_tuple(AttackTypeNext, DamageNext, false);
			}

			bool nextAttackMatchesInput = Combos[CurrentCombo].AttackMatchesNextAttack(AttackType, CurrentComboStep + 1);
			
			if (!nextAttackMatchesInput) {
				CurrentCombo = GetNextCombo(AttackType);
			}

			if (UpcomingAttackAnims.size() > 0) {
				auto& [AttackAnimType, ClosenessToBeat, ShouldAttack] = UpcomingAttackAnims.front();

				if (ShouldAttack) {
					UpcomingAttackAnims.pop();
					PerformAnimation(AttackAnimType, ClosenessToBeat, false);
				}
				else {
					UpcomingAttackAnims.front() = std::make_tuple(AttackAnimType, ClosenessToBeat, true);
				}
			}
			
			PerformAttack(AttackType);
		}
	}
	else {
		for (auto& combo : Combos) {
			combo.ResetCombo();
			CurrentCombo = 0;
			CurrentComboStep = -1;
		}
	}
}

void UComboManagerComponent::BindAttackCallbackFunc(APlayerCharacter* playerCharacter, AttackCallback callbackFunc)
{
	player = playerCharacter;
	callback = callbackFunc;
}

void UComboManagerComponent::PerformAttack(Attacks AttackType)
{
	CurrentComboStep = Combos[CurrentCombo].NextAttack(CurrentComboStep);

	if (player) {
		(player->*callback)(AttackType, Combos[CurrentCombo].GetMotionValue(CurrentComboStep),
			Combos[CurrentCombo].GetAnimLength(CurrentComboStep), CurrentCombo, CurrentComboStep);
	}

	if (StoredAttacks.size() == 0) {
		if (Combos[CurrentCombo].AttackMatchesNextAttack(Attacks::Attack_Pause, CurrentComboStep + 1)) {
			AddAttack(Attacks::Attack_Pause, 0, false);
		}
		else {
			int potentialCombo = GetNextCombo(Attacks::Attack_Pause, false);

			if (potentialCombo > 0) {
				CurrentCombo = potentialCombo;
				AddAttack(Attacks::Attack_Pause, 0, false);
			}
		}
	}
}

void UComboManagerComponent::PerformAnimation(Attacks AttackType, float ClosenessToBeat, bool AddTimeBetweenBeats)
{
	switch (AttackType) {
	case Attacks::Attack_Neutral:
		if (ClosenessToBeat > ClosenessPercentForPerfectBeat) {
			player->PlayAttackMontage(TEXT("PerfectNeutral"), AddTimeBetweenBeats);
		}
		else {
			player->PlayAttackMontage(TEXT("AttackNeutral"), AddTimeBetweenBeats);
		}
		break;

	case Attacks::Attack_Type1:
		if (ClosenessToBeat > ClosenessPercentForPerfectBeat) {
			player->PlayAttackMontage(TEXT("Perfect1"), AddTimeBetweenBeats);
		}
		else {
			player->PlayAttackMontage(TEXT("Attack1"), AddTimeBetweenBeats);
		}
		break;

	default:
		if (ClosenessToBeat > ClosenessPercentForPerfectBeat) {
			player->PlayAttackMontage(TEXT("PerfectNeutral"), AddTimeBetweenBeats);
		}
		else {
			player->PlayAttackMontage(TEXT("AttackNeutral"), AddTimeBetweenBeats);
		}
		break;
	}
}

