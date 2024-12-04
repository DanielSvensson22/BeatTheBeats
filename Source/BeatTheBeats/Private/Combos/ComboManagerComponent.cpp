// Fill out your copyright notice in the Description page of Project Settings.


#include "Combos/ComboManagerComponent.h"
#include "Beats/BeatManager.h"
#include "Kismet/GameplayStatics.h"
#include "Combos/Combo.h"
#include <tuple>
#include <queue>
#include "Character/PlayerCharacter.h"
#include "Character/BeatTheBeatsPlayerController.h"
#include "Weapons/WeaponBase.h"
#include "Blueprint/UserWidget.h"
#include "Blueprint/WidgetTree.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"

// Sets default values for this component's properties
UComboManagerComponent::UComboManagerComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...
}

UComboManagerComponent::~UComboManagerComponent()
{
	if (BeatManager) {
		for (auto& handle : Handles) {
			BeatManager->UnBindFuncFromOnBeat(handle);
		}
	}	
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
		FDelegateHandle h1 = BeatManager->BindFuncToOnBeat(this, &UComboManagerComponent::ProcessNextAttack);
		FDelegateHandle h2 = BeatManager->BindFuncToOnBeat(this, &UComboManagerComponent::ResetTimeOfLastAttack);
		Handles.Add(h1);
		Handles.Add(h2);
	}

	if (Combos.Num() != ComboObtainedStatus.Num()) {
		UE_LOG(LogTemp, Error, TEXT("Combo count not equal to combo obtained status count!"));
	}

	if (ComboConnectionsList.Num() != Combos.Num()) {
		UE_LOG(LogTemp, Error, TEXT("Not every combo has a combo connections list!"));
	}

	if (player && player->GetController()) {
		if (ABeatTheBeatsPlayerController* controller = Cast<ABeatTheBeatsPlayerController>(player->GetController())) {
			ComboText = Cast<UTextBlock>(controller->GetHUD()->WidgetTree->FindWidget(TEXT("ComboText")));

			UImage* indicator1 = Cast<UImage>(controller->GetHUD()->WidgetTree->FindWidget(TEXT("ComboImage1")));

			if (indicator1) {
				ComboImages.Add(indicator1);
			}

			UImage* indicator2 = Cast<UImage>(controller->GetHUD()->WidgetTree->FindWidget(TEXT("ComboImage2")));

			if (indicator2) {
				ComboImages.Add(indicator2);
			}

			UImage* indicator3 = Cast<UImage>(controller->GetHUD()->WidgetTree->FindWidget(TEXT("ComboImage3")));

			if (indicator3) {
				ComboImages.Add(indicator3);
			}

			UImage* indicator4 = Cast<UImage>(controller->GetHUD()->WidgetTree->FindWidget(TEXT("ComboImage4")));

			if (indicator4) {
				ComboImages.Add(indicator4);
			}
		}
	}

	SetComboIndicators();
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

				SetComboIndicators();
			}
		}
		else {
			for (auto& combo : Combos) {
				combo.ResetCombo();
				CurrentCombo = 0;
				CurrentComboStep = -1;
			}

			SetComboIndicators();
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
		if (Combos[CurrentCombo].HasQTE() && CurrentComboStep == Combos[CurrentCombo].AttackCount() - 1) {
			(player->*callback)(Combos[CurrentCombo].GetQTEDescription(), Combos[CurrentCombo].GetComboEffect());
		}
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
	if (player) {
		if (ClosenessToBeat > ClosenessPercentForPerfectBeat) {
			player->PlayAttackMontage(montage, TEXT("Perfect"), BeatManager->TimeBetweenBeats() * 0.9f);
		}
		else {
			player->PlayAttackMontage(montage, TEXT("Default"), BeatManager->TimeBetweenBeats() * 0.9f);
		}
	}
}

void UComboManagerComponent::SetComboIndicators()
{
	if (ComboText) {
		ComboText->SetText(Combos[CurrentCombo].GetName());
	}

	for (int i = 0; i < ComboImages.Num(); i++) {
		if (i <= CurrentComboStep) {
			ComboImages[i]->SetRenderScale(FVector2D(1, 1));

			switch (Combos[CurrentCombo].GetAttackType(i)) {
			case Attacks::Attack_Neutral:
				ComboImages[i]->SetBrushTintColor(ActiveNeutral);
				break;

			case Attacks::Attack_Type1:
				ComboImages[i]->SetBrushTintColor(ActiveOne);
				break;

			case Attacks::Attack_Type2:
				ComboImages[i]->SetBrushTintColor(ActiveTwo);
				break;

			case Attacks::Attack_Type3:
				ComboImages[i]->SetBrushTintColor(ActiveThree);
				break;

			default:
				ComboImages[i]->SetBrushTintColor(ActiveNeutral);
				break;
			}
		}
		else {
			ComboImages[i]->SetRenderScale(FVector2D(0.5f, 0.5f));

			switch (Combos[CurrentCombo].GetAttackType(i)) {
			case Attacks::Attack_Neutral:
				ComboImages[i]->SetBrushTintColor(DeactivatedNeutral);
				break;

			case Attacks::Attack_Type1:
				ComboImages[i]->SetBrushTintColor(DeactivatedOne);
				break;

			case Attacks::Attack_Type2:
				ComboImages[i]->SetBrushTintColor(DeactivatedTwo);
				break;

			case Attacks::Attack_Type3:
				ComboImages[i]->SetBrushTintColor(DeactivatedThree);
				break;

			default:
				ComboImages[i]->SetBrushTintColor(DeactivatedNeutral);
				break;
			}
		}
	}
}