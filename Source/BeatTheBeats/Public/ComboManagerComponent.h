// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Combo.h"
#include <tuple>
#include <queue>
#include "ComboManagerComponent.generated.h"


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
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void AddAttack(Attacks AttackType, float Damage);

	void ProcessNextAttack(float CurrentTimeSinceLastBeat);

private:

	typedef std::tuple<Attacks, float, bool> StoredAttack;

	UPROPERTY(EditDefaultsOnly)
	TArray<FCombo> Combos;

	std::queue<StoredAttack> StoredAttacks;

	int CurrentComboStep = -1;

	int CurrentCombo = 0;

	class ABeatManager* BeatManager;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<ABeatManager> BeatManagerClass;
};
