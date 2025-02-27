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
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}

UComboManagerComponent::~UComboManagerComponent()
{
#if WITH_EDITOR
	if (BeatManager) {
		for (auto& handle : Handles) {
			BeatManager->UnBindFuncFromOnBeat(handle);
		}
	}	
#endif
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

			controller->ForceInit();
			if (controller->GetHUD()) {
				ComboText = Cast<UTextBlock>(controller->GetHUD()->WidgetTree->FindWidget(TEXT("ComboText")));
				OnBeatText = Cast<UTextBlock>(controller->GetHUD()->WidgetTree->FindWidget(TEXT("OnBeatText")));

				if (OnBeatText) {
					OnBeatText->SetVisibility(ESlateVisibility::Hidden);
				}

				/*UImage* indicator1 = Cast<UImage>(controller->GetHUD()->WidgetTree->FindWidget(TEXT("ComboImage1")));

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
				}*/

				for (int i = 0; i < 4; i++) {
					FString name("ND_");
					name.AppendInt(i);

					UImage* indicator = Cast<UImage>(controller->GetHUD()->WidgetTree->FindWidget(FName(name)));

					if (indicator) {
						ComboImages.Add(indicator);
					}
				}

				for (int i = 0; i < 4; i++) {
					FString name("NA_");
					name.AppendInt(i);

					UImage* indicator = Cast<UImage>(controller->GetHUD()->WidgetTree->FindWidget(FName(name)));

					if (indicator) {
						ComboImages.Add(indicator);
					}
				}

				for (int i = 0; i < 4; i++) {
					FString name("1D_");
					name.AppendInt(i);

					UImage* indicator = Cast<UImage>(controller->GetHUD()->WidgetTree->FindWidget(FName(name)));

					if (indicator) {
						ComboImages.Add(indicator);
					}
				}

				for (int i = 0; i < 4; i++) {
					FString name("1A_");
					name.AppendInt(i);

					UImage* indicator = Cast<UImage>(controller->GetHUD()->WidgetTree->FindWidget(FName(name)));

					if (indicator) {
						ComboImages.Add(indicator);
					}
				}

				for (int i = 0; i < 4; i++) {
					FString name("2D_");
					name.AppendInt(i);

					UImage* indicator = Cast<UImage>(controller->GetHUD()->WidgetTree->FindWidget(FName(name)));

					if (indicator) {
						ComboImages.Add(indicator);
					}
				}

				for (int i = 0; i < 4; i++) {
					FString name("2A_");
					name.AppendInt(i);

					UImage* indicator = Cast<UImage>(controller->GetHUD()->WidgetTree->FindWidget(FName(name)));

					if (indicator) {
						ComboImages.Add(indicator);
					}
				}

				for (int i = 0; i < 4; i++) {
					FString name("3D_");
					name.AppendInt(i);

					UImage* indicator = Cast<UImage>(controller->GetHUD()->WidgetTree->FindWidget(FName(name)));

					if (indicator) {
						ComboImages.Add(indicator);
					}
				}

				for (int i = 0; i < 4; i++) {
					FString name("3A_");
					name.AppendInt(i);

					UImage* indicator = Cast<UImage>(controller->GetHUD()->WidgetTree->FindWidget(FName(name)));

					if (indicator) {
						ComboImages.Add(indicator);
					}
				}
			}		
		}
	}

	SetComboIndicators();
}


// Called every frame
void UComboManagerComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (BeatTextTime > 0) {
		BeatTextTime -= DeltaTime;

		if (OnBeatText) {
			OnBeatText->SetOpacity(BeatTextTime / BeatTextLifeTime);
		}

		if (OnBeatText && BeatTextTime <= 0) {
			OnBeatText->SetVisibility(ESlateVisibility::Hidden);
		}
	}
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
							bool OnBeat = ClosenessToBeat > ClosenessPercentForPerfectBeat;
							Weapon->SetAttackStatus(Damage, AttackType, OnBeat);

							if (OnBeat) {
								SetBeatText("On Beat!", FLinearColor::Blue);
							}
							else {
								SetBeatText("Off Beat...", FLinearColor::Red);
							}
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

void UComboManagerComponent::SetBeatText(const char* text, FLinearColor color)
{
	if (OnBeatText) {
		OnBeatText->SetText(FText::FromString(FString(text)));
		OnBeatText->SetColorAndOpacity(color);

		OnBeatText->SetVisibility(ESlateVisibility::Visible);
		OnBeatText->SetOpacity(1);

		BeatTextTime = BeatTextLifeTime;
	}
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

	if (ComboImages.Num() < 32) {
		UE_LOG(LogTemp, Error, TEXT("Not enough combo images!"));
		return;
	}

	DeactivateComboImages(0, ComboImages.Num() - 1);

	for (int i = 0; i < ComboImages.Num(); i++) {
		if ((i % 4) <= CurrentComboStep) {
			ComboImages[i]->SetRenderScale(FVector2D(1, 1));

			if (AttackByIndex(i) == Combos[CurrentCombo].GetAttackType(i % 4)) {
				FString name;
				ComboImages[i]->GetName(name);

				if (name[1] == 'A') {
					ActivateComboImage(i);
				}
			}
			else {
				if (AttackByIndex(i) == Attacks::Attack_NONE) {
					UE_LOG(LogTemp, Error, TEXT("Combo image index %i was not in acceptable range"), i);
				}
			}

			/*switch (Combos[CurrentCombo].GetAttackType(i)) {
			case Attacks::Attack_Neutral:
				ComboImages[i]->SetBrushTintColor(ActiveNeutral);
				ComboImages[i]->SetColorAndOpacity(ActiveNeutral);

				break;

			case Attacks::Attack_Type1:
				ComboImages[i]->SetBrushTintColor(ActiveOne);
				ComboImages[i]->SetColorAndOpacity(ActiveOne);
				break;

			case Attacks::Attack_Type2:
				ComboImages[i]->SetBrushTintColor(ActiveTwo);
				ComboImages[i]->SetColorAndOpacity(ActiveTwo);
				break;

			case Attacks::Attack_Type3:
				ComboImages[i]->SetBrushTintColor(ActiveThree);
				ComboImages[i]->SetColorAndOpacity(ActiveThree);
				break;

			default:
				ComboImages[i]->SetBrushTintColor(ActiveNeutral);
				ComboImages[i]->SetColorAndOpacity(ActiveNeutral);
				break;
			}*/
		}
		else {
			ComboImages[i]->SetRenderScale(FVector2D(0.5f, 0.5f));

			if (AttackByIndex(i) == Combos[CurrentCombo].GetAttackType(i % 4)) {
				FString name;
				ComboImages[i]->GetName(name);

				if (name[1] == 'D') {
					ActivateComboImage(i);
				}
			}
			else {
				if (AttackByIndex(i) == Attacks::Attack_NONE) {
					UE_LOG(LogTemp, Error, TEXT("Combo image index %i was not in acceptable range"), i);
				}
			}

			/*switch (Combos[CurrentCombo].GetAttackType(i)) {
			case Attacks::Attack_Neutral:
				ComboImages[i]->SetBrushTintColor(DeactivatedNeutral);
				ComboImages[i]->SetColorAndOpacity(DeactivatedNeutral);
				break;

			case Attacks::Attack_Type1:
				ComboImages[i]->SetBrushTintColor(DeactivatedOne);
				ComboImages[i]->SetColorAndOpacity(DeactivatedOne);
				break;

			case Attacks::Attack_Type2:
				ComboImages[i]->SetBrushTintColor(DeactivatedTwo);
				ComboImages[i]->SetColorAndOpacity(DeactivatedTwo);
				break;

			case Attacks::Attack_Type3:
				ComboImages[i]->SetBrushTintColor(DeactivatedThree);
				ComboImages[i]->SetColorAndOpacity(DeactivatedThree);
				break;

			default:
				ComboImages[i]->SetBrushTintColor(DeactivatedNeutral);
				ComboImages[i]->SetColorAndOpacity(DeactivatedNeutral);
				break;
			}
		}*/
		}

		ComboImages[i]->SynchronizeProperties();
	}
}

void UComboManagerComponent::DeactivateComboImages(int start, int end)
{
	for (int i = start; i <= end; i++) {
		ComboImages[i]->SetVisibility(ESlateVisibility::Hidden);
		ComboImages[i]->SetIsEnabled(false);
	}
}

void UComboManagerComponent::ActivateComboImage(int index)
{
	ComboImages[index]->SetVisibility(ESlateVisibility::Visible);
	ComboImages[index]->SetIsEnabled(true);
}

Attacks UComboManagerComponent::AttackByIndex(int index)
{
	if (index >= 0 && index < 8) {
		return Attacks::Attack_Neutral;
	}
	else if (index >= 8 && index < 16) {
		return Attacks::Attack_Type1;
	}
	else if (index >= 16 && index < 24) {
		return Attacks::Attack_Type2;
	}
	else if (index >= 24 && index < 32) {
		return Attacks::Attack_Type3;
	}

	return Attacks::Attack_NONE;
}
