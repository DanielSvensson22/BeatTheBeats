// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Combo.h"
#include <tuple>
#include <queue>
#include "ComboConnection.h"
#include "ComboConnectionArray.h"
#include "ComboManagerComponent.generated.h"

class APlayerCharacter;
class AWeaponBase;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class BEATTHEBEATS_API UComboManagerComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UComboManagerComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	typedef void (APlayerCharacter::* AttackCallback)(Attacks, float, float, int, int);

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

	void PerformAttack(Attacks AttackType);

	void PerformAnimation(Attacks AttackType, float ClosenessToBeat, bool AddTimeBetweenBeats);

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

	class ABeatManager* BeatManager;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<ABeatManager> BeatManagerClass;

	APlayerCharacter* player;
	AttackCallback callback;

	UPROPERTY(EditDefaultsOnly)
	float ClosenessPercentForPerfectBeat = 0.8f;

	AWeaponBase* Weapon;
};
