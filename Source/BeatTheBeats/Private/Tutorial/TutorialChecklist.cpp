// Fill out your copyright notice in the Description page of Project Settings.


#include "Tutorial/TutorialChecklist.h"

void UTutorialChecklist::MarkStepCompleted(ETutorialStep Step)
{
    int32 Index = static_cast<int32>(Step);
    if (TutorialProgress.IsValidIndex(Index) && !TutorialProgress[Index])
    {
        if (!TutorialProgress[Index]) 
        {
            // Broadcast the updated step
            if (OnTutorialStepCompleted.IsBound()) 
            {
                OnTutorialStepCompleted.Broadcast(Step); 
                //UE_LOG(LogTemp, Warning, TEXT("BroadCasting"));
            }
            else
            {
                UE_LOG(LogTemp, Warning, TEXT("OnTutorialStepCompleted delegate is not bound!")); 
            }
        }
        TutorialProgress[Index] = true; 
    }
}

bool UTutorialChecklist::IsStepCompleted(ETutorialStep Step) const
{
    int32 Index = static_cast<int32>(Step);
    return TutorialProgress.IsValidIndex(Index) ? TutorialProgress[Index] : false;
}

void UTutorialChecklist::ResetChecklist()
{
    for (int i = 0; i < static_cast<int32>(ETutorialStep::Combo1323); i++)
    {
        if (TutorialProgress.IsValidIndex(i)) TutorialProgress[i] = false;
    }
}
