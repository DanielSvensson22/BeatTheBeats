// Fill out your copyright notice in the Description page of Project Settings.


#include "Combos/ComboManagerComponent.h"
#include "Beats/BeatManager.h"
#include "Kismet/GameplayStatics.h"
#include "Combos/Combo.h"
#include <tuple>
#include <queue>
#include "Character/PlayerCharacter.h"
#include "Weapons/WeaponBase.h"

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
		BeatManager->BindFuncToOnBeat(this, &UComboManagerComponent::ResetTimeOfLastAttack);
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
	if (StoredAttacks.size() == 0) {
		if (BeatManager && player && 
			(!player->InAttackAnimation() || 
				(TimeOfLastAttack == 0 || std::abs(BeatManager->GetCurrentTimeSinceLastBeat() - TimeOfLastAttack) > BeatManager->TimeBetweenBeats() / BeatPartBeforeNewAttack))) {
			StoredAttacks.emplace(AttackType, Damage, false);
			TimeOfLastAttack = BeatManager->GetCurrentTimeSinceLastBeat();

			if (AttackType != Attacks::Attack_Pause) {
				AddAttackAnim(AttackType);

				bProcessedThisBeat = false;
				ProcessNextAttack(BeatManager->GetCurrentTimeSinceLastBeat());
				bProcessedThisBeat = true;
			}
		}
	}
}

void UComboManagerComponent::ProcessNextAttack(float CurrentTimeSinceLastBeat)
{
	if (!bProcessedThisBeat) {
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

						if (AttackAnimType != Attacks::Attack_Pause) {
							PerformAnimation(AttackAnimType, ClosenessToBeat, false,
								Combos[CurrentCombo].GetAnimMontage(Combos[CurrentCombo].GetNextAttack(CurrentComboStep)));
						}

						if (Weapon) {
							Weapon->SetAttackStatus(Damage, AttackType, ClosenessToBeat > ClosenessPercentForPerfectBeat);
						}
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
	else {
		bProcessedThisBeat = false;
	}
}

void UComboManagerComponent::BindAttackCallbackFunc(APlayerCharacter* playerCharacter, AttackCallback callbackFunc)
{
	player = playerCharacter;
	callback = callbackFunc;
}

void UComboManagerComponent::SetWeapon(AWeaponBase* PlayerWeapon)
{
	Weapon = PlayerWeapon;
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

void UComboManagerComponent::PerformAnimation(Attacks AttackType, float ClosenessToBeat, bool AddTimeBetweenBeats, UAnimMontage* montage)
{
	if (ClosenessToBeat > ClosenessPercentForPerfectBeat) {
		player->PlayAttackMontage(montage, TEXT("Perfect"), BeatManager->TimeBetweenBeats() * 0.9f);
	}
	else {
		player->PlayAttackMontage(montage, TEXT("Default"), BeatManager->TimeBetweenBeats() * 0.9f);
	}

	UE_LOG(LogTemp, Display, TEXT("Animated on combo %i step %i"), CurrentCombo, FMath::Max(CurrentComboStep, 0));
}

