// Fill out your copyright notice in the Description page of Project Settings.


#include "10_Enemy/BTTask_AOEAttack.h"

#include "10_Enemy/BossEnemy.h"
#include "AIController.h"

UBTTask_AOEAttack::UBTTask_AOEAttack()
{
    NodeName = "Perform AOE Attack";
}

EBTNodeResult::Type UBTTask_AOEAttack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    AAIController* AIController = OwnerComp.GetAIOwner();
    if (!AIController)
    {
        UE_LOG(LogTemp, Error, TEXT("AIController is null in AOE Attack Task"));
        return EBTNodeResult::Failed;
    }

    ABossEnemy* Boss = Cast<ABossEnemy>(AIController->GetPawn());
    if (!Boss)
    {
        UE_LOG(LogTemp, Error, TEXT("BossEnemy is null in AOE Attack Task"));
        return EBTNodeResult::Failed;
    }

    Boss->PerformAOEAttack();

    return EBTNodeResult::Succeeded;
}