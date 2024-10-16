// Fill out your copyright notice in the Description page of Project Settings.


#include "10_Enemy/PeCoFlyingEnemyAIController.h"

#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/BehaviorTree.h"
#include "Kismet/GameplayStatics.h"

void APeCoFlyingEnemyAIController::BeginPlay()
{
    Super::BeginPlay();
    
    if (FlyingAIBehavior != nullptr)
    {
        if (UseBlackboard(FlyingAIBehavior->BlackboardAsset, BlackboardComponent))
        {
            RunBehaviorTree(FlyingAIBehavior);
        }
    }
}

void APeCoFlyingEnemyAIController::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);

    APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
    if (PlayerPawn && BlackboardComponent)
    {
        
        if (LineOfSightTo(PlayerPawn))
        {
            SetFocus(PlayerPawn);
            BlackboardComponent->SetValueAsVector(TEXT("PlayerLocation"), PlayerPawn->GetActorLocation());            
        }
        else
        {
            ClearFocus(EAIFocusPriority::Gameplay); 
            BlackboardComponent->ClearValue(TEXT("PlayerLocation"));
        }
    }
}