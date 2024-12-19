// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Combo.h"
#include <tuple>
#include <queue>
#include "ComboConnection.h"
#include "ComboConnectionArray.h"
#include "Beats/BeatManager.h"
#include "ComboManagerComponent.generated.h"

class APlayerCharacter;
class AWeaponBase;
class UTextBlock;
class UImage;

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class BEATTHEBEATS_API UComboManagerComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UComboManagerComponent();
	~UComboManagerComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	typedef void (APlayerCharacter::* AttackCallback)(TArray<FQTEDescription>*, ComboEffect);

	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void AddAttack(Attacks AttackType, float Damage, bool PlayerAddedThisBeat = true);

	void ProcessNextAttack(float CurrentTimeSinceLastBeat);

	void BindAttackCallbackFunc(APlayerCharacter* playerCharacter, AttackCallback callbackFunc);

	void SetWeapon(AWeaponBase* PlayerWeapon);

private:

	FORCEINLINE int GetNextCombo(Attacks AttackType, bool ResetComboStep = true) {
		int NextCombo = -1;

		for (int i = 0; i < ComboConnectionsList[CurrentCombo].Connections.Num(); i++) {
			FComboConnection& cc = ComboConnectionsList[CurrentCombo].Connections[i];

			if (ComboObtainedStatus[cc.GetToCombo()] == true) {
				if (cc.GetFromComboStep() == CurrentComboStep) {
					if (cc.GetAttackWhichConnects() == AttackType) {
						NextCombo = cc.GetToCombo();
					}
				}
			}
		}

		if (NextCombo > -1) {
			return NextCombo;
		}

		switch (AttackType) {
		case Attacks::Attack_Neutral:
			if (ResetComboStep) {
				CurrentComboStep = -1;
			}

			return BaseNeutralCombo;
			break;

		case Attacks::Attack_Type1:
			if (ResetComboStep) {
				CurrentComboStep = -1;
			}

			return BaseAttack1Combo;
			break;

		case Attacks::Attack_Type2:
			if (ResetComboStep) {
				CurrentComboStep = -1;
			}

			return BaseAttack2Combo;
			break;

		case Attacks::Attack_Type3:
			if (ResetComboStep) {
				CurrentComboStep = -1;
			}

			return BaseAttack3Combo;
			break;

		default:
			if (ResetComboStep) {
				CurrentComboStep = -1;
			}

			return 0;
			break;
		}

		//return 0;
	}

	FORCEINLINE int FindComboOfType(Attacks Type) {
		for (int i = 0; i < Combos.Num(); i++) {
			if (Combos[i].GetAttackType(0) == Type) {
				return i;
			}
		}

		return 0;
	}

	FORCEINLINE void AddAttackAnim(Attacks AttackType) {
		if (UpcomingAttackAnims.size() < 2) {
			UpcomingAttackAnims.emplace(AttackType, BeatManager->ClosenessToBeat(), true);
		}
	}

	FORCEINLINE void ResetTimeOfLastAttack(float CurrentTimeSinceLastBeat) {
		TimeOfLastAttack = 0;
	}

	void PerformAttack(Attacks AttackType);

	void PerformAnimation(Attacks AttackType, float ClosenessToBeat, bool AddTimeBetweenBeats, UAnimMontage* montage);

	void SetComboIndicators();

	void DeactivateComboImages(int start, int end);
	void ActivateComboImage(int index);

	Attacks AttackByIndex(int index);

private:

	typedef std::tuple<Attacks, float, bool> StoredAttack;

	UPROPERTY(EditDefaultsOnly)
		TArray<FCombo> Combos;

	UPROPERTY(EditDefaultsOnly)
		TArray<bool> ComboObtainedStatus;

	UPROPERTY(EditDefaultsOnly)
		TArray<FComboConnectionArray> ComboConnectionsList;

	UPROPERTY(EditDefaultsOnly)
		int BaseNeutralCombo;

	UPROPERTY(EditDefaultsOnly)
		int BaseAttack1Combo;

	UPROPERTY(EditDefaultsOnly)
		int BaseAttack2Combo;

	UPROPERTY(EditDefaultsOnly)
		int BaseAttack3Combo;

	std::queue<StoredAttack> StoredAttacks;

	std::queue<StoredAttack> UpcomingAttackAnims;

	int CurrentComboStep = -1;

	int CurrentCombo = 0;

	UPROPERTY()
		ABeatManager* BeatManager;

	UPROPERTY(EditDefaultsOnly)
		TSubclassOf<ABeatManager> BeatManagerClass;

	APlayerCharacter* player;
	AttackCallback callback;

	bool bProcessedThisBeat = false;

	float TimeOfLastAttack = 0;

	UPROPERTY(EditDefaultsOnly)
		float ClosenessPercentForPerfectBeat = 0.8f;

	UPROPERTY(EditDefaultsOnly)
		float BeatPartBeforeNewAttack = 5.0f;

	AWeaponBase* Weapon;

	TArray<FDelegateHandle> Handles;

	//Combo indicators
	UTextBlock* ComboText;

	TArray<UImage*> ComboImages;

	UPROPERTY(EditDefaultsOnly, Category = "Combo indicator")
		FLinearColor ActiveNeutral;

	UPROPERTY(EditDefaultsOnly, Category = "Combo indicator")
		FLinearColor DeactivatedNeutral;

	UPROPERTY(EditDefaultsOnly, Category = "Combo indicator")
		FLinearColor ActiveOne;

	UPROPERTY(EditDefaultsOnly, Category = "Combo indicator")
		FLinearColor DeactivatedOne;

	UPROPERTY(EditDefaultsOnly, Category = "Combo indicator")
		FLinearColor ActiveTwo;

	UPROPERTY(EditDefaultsOnly, Category = "Combo indicator")
		FLinearColor DeactivatedTwo;

	UPROPERTY(EditDefaultsOnly, Category = "Combo indicator")
		FLinearColor ActiveThree;

	UPROPERTY(EditDefaultsOnly, Category = "Combo indicator")
		FLinearColor DeactivatedThree;
};