// Fill out your copyright notice in the Description page of Project Settings.


#include "10_Enemy/BTTask_ReturnToSquare.h"

#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "10_Enemy/BossEnemy.h"

UBTTask_ReturnToSquare::UBTTask_ReturnToSquare()
{
    NodeName = "Return to Square";
}

EBTNodeResult::Type UBTTask_ReturnToSquare::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    AAIController* AIController = OwnerComp.GetAIOwner();
    if (!AIController)
    {
        UE_LOG(LogTemp, Error, TEXT("AIController is null in ReturnToSquare task"));
        return EBTNodeResult::Failed;
    }

    ABossEnemy* Boss = Cast<ABossEnemy>(AIController->GetPawn());
    if (!Boss)
    {
        UE_LOG(LogTemp, Error, TEXT("BossEnemy is null in ReturnToSquare task"));
        return EBTNodeResult::Failed;
    }

    // 현재 보스 위치
    FVector CurrentLocation = Boss->GetActorLocation();

    // 가장 가까운 테두리 지점 계산
    FVector ClosestEdgePoint = GetClosestEdgePoint(CurrentLocation);

    // 보스를 순간이동 시킴
    Boss->SetActorLocation(ClosestEdgePoint);

    // 블랙보드에 새로운 이동 지점을 저장 (반시계 이동을 이어가기 위해)
    UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();
    if (BlackboardComp)
    {
        BlackboardComp->SetValueAsVector(GetSelectedBlackboardKey(), ClosestEdgePoint);
    }

    UE_LOG(LogTemp, Log, TEXT("Boss returned to square edge: %s"), *ClosestEdgePoint.ToString());
    return EBTNodeResult::Succeeded;
}

FVector UBTTask_ReturnToSquare::GetClosestEdgePoint(const FVector& CurrentLocation) const
{
    // 사각형의 각 테두리 좌표
    FVector Points[4] = {
        SquareCenter + FVector(-SquareSize, -SquareSize, 0), // Bottom-left
        SquareCenter + FVector(-SquareSize, SquareSize, 0),  // Top-left
        SquareCenter + FVector(SquareSize, SquareSize, 0),   // Top-right
        SquareCenter + FVector(SquareSize, -SquareSize, 0)   // Bottom-right
    };

    // 현재 위치에서 가장 가까운 테두리 점 계산
    int32 ClosestIndex = 0;
    float ClosestDistance = FVector::Dist(CurrentLocation, Points[0]);
    for (int32 i = 1; i < 4; ++i)
    {
        float Distance = FVector::Dist(CurrentLocation, Points[i]);
        if (Distance < ClosestDistance)
        {
            ClosestDistance = Distance;
            ClosestIndex = i;
        }
    }

    return Points[ClosestIndex];
}