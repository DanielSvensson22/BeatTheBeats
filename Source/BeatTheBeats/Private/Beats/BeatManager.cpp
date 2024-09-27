// Fill out your copyright notice in the Description page of Project Settings.


#include "Beats/BeatManager.h"

// Sets default values
ABeatManager::ABeatManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ABeatManager::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ABeatManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	CurrentTimeSinceLastBeat += DeltaTime;

	if (CurrentTimeSinceLastBeat >= TimeBetweenBeats()) {
		CurrentTimeSinceLastBeat -= TimeBetweenBeats();
		OnBeat.Broadcast(CurrentTimeSinceLastBeat);
	}
}

