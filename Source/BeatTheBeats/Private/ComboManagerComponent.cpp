// Fill out your copyright notice in the Description page of Project Settings.


#include "ComboManagerComponent.h"
#include "BeatManager.h"
#include "Kismet/GameplayStatics.h"
#include "Combo.h"
#include <tuple>
#include <queue>

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
}


// Called every frame
void UComboManagerComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UComboManagerComponent::AddAttack(Attacks AttackType, float Damage)
{
	if (StoredAttacks.size() < 2) {
		StoredAttacks.emplace(AttackType, Damage, true);
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
			
			int OriginalCombo = CurrentCombo;
			bool ComboHasNotLooped = true;
			while (!nextAttackMatchesInput) {
				CurrentCombo++;

				if (CurrentCombo >= Combos.Num()) {
					CurrentCombo = 0;
					ComboHasNotLooped = false;
				}

				if (ComboHasNotLooped && CurrentCombo > 0 && CurrentComboStep > 0) {
					if (Combos[OriginalCombo].GetAttackType(CurrentComboStep - 1) != Combos[CurrentCombo].GetAttackType(CurrentComboStep - 1)) {
						continue;
					}
				}

				nextAttackMatchesInput = Combos[CurrentCombo].AttackMatchesNextAttack(AttackType, CurrentComboStep + 1);
			}
			
			CurrentComboStep = Combos[CurrentCombo].NextAttack(CurrentComboStep);
			//To Do: Add functionality for dealing damage...
			FString attackName;

			switch (AttackType) {
			case Attacks::LightAttack:
				attackName = "Light Attack";
				break;

			case Attacks::HeavyAttack:
				attackName = "Heavy Attack";
				break;

			default:
				UE_LOG(LogTemp, Error, TEXT("Attack type not implemented!"));
			}

			UE_LOG(LogTemp, Warning, TEXT("Landed %s on Combo step %i in Combo %i"), *attackName, CurrentComboStep, CurrentCombo);
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

