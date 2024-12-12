// Fill out your copyright notice in the Description page of Project Settings.


#include "Tutorial/TutorialChecklist.h"

void UTutorialChecklist::MarkStepCompleted(ETutorialStep Step)
{
    int32 Index = static_cast<int32>(Step);
    if (TutorialProgress.IsValidIndex(Index) && !TutorialProgress[Index])
    {
        TutorialProgress[Index] = true;

        // Broadcast the updated step
        if (OnTutorialStepCompleted.IsBound())
        {
            OnTutorialStepCompleted.Broadcast(Step);
        }
        else
        {
            UE_LOG(LogTemp, Warning, TEXT("OnTutorialStepCompleted delegate is not bound!"));
        }
    }
}

bool UTutorialChecklist::IsStepCompleted(ETutorialStep Step) const
{
    int32 Index = static_cast<int32>(Step);
    return TutorialProgress.IsValidIndex(Index) ? TutorialProgress[Index] : false;
}