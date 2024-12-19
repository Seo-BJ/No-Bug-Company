// Fill out your copyright notice in the Description page of Project Settings.


#include "10_Enemy/BTTask/BTTask_ReturnToSquare.h"

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

    FVector CurrentLocation = Boss->GetActorLocation();

    // 사각형 중심과 크기 가져오기
    FVector SquareCenter = Boss->CenterVentLocation;   
    float SquareSize = Boss->SquareSize;

    // 가장 가까운 테두리 점 계산
    FVector ClosestEdgePoint = GetClosestEdgePoint(CurrentLocation, SquareCenter, SquareSize);

    // 블랙보드에 가장 가까운 테두리 점 저장
    UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();
    if (BlackboardComp)
    {
        BlackboardComp->SetValueAsVector("EdgeLocation", ClosestEdgePoint);
    }

    // 반시계 방향의 꼭짓점 계산
    FVector CounterClockwiseCorner = GetCounterClockwiseCorner(ClosestEdgePoint, SquareCenter, SquareSize);
    if (BlackboardComp)
    {
        BlackboardComp->SetValueAsVector("CornerLocation", CounterClockwiseCorner);
    }

    UE_LOG(LogTemp, Log, TEXT("Closest Edge Point: %s"), *ClosestEdgePoint.ToString());
    UE_LOG(LogTemp, Log, TEXT("Next CounterClockwise Corner: %s"), *CounterClockwiseCorner.ToString());

    return EBTNodeResult::Succeeded;             
       

}

FVector UBTTask_ReturnToSquare::GetClosestEdgePoint(const FVector& CurrentLocation, const FVector& SquareCenter, float SquareSize) const
{
    // 정사각형 테두리의 포인트 설정
    FVector Points[8] = {
        // Bottom edge
        SquareCenter + FVector(-SquareSize, -SquareSize, 0), // Bottom-left corner
        SquareCenter + FVector(0, -SquareSize, 0),           // Bottom-center
        SquareCenter + FVector(SquareSize, -SquareSize, 0),  // Bottom-right corner

        // Right edge
        SquareCenter + FVector(SquareSize, 0, 0),            // Right-center
        SquareCenter + FVector(SquareSize, SquareSize, 0),   // Top-right corner

        // Top edge
        SquareCenter + FVector(0, SquareSize, 0),            // Top-center
        SquareCenter + FVector(-SquareSize, SquareSize, 0),  // Top-left corner

        // Left edge
        SquareCenter + FVector(-SquareSize, 0, 0)            // Left-center     
    };

    // 현재 위치에서 가장 가까운 포인트 찾기
    int32 ClosestIndex = 0;
    float ClosestDistance = FVector::Dist(CurrentLocation, Points[0]);

    for (int32 i = 1; i < 8; ++i)
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

// 반시계 방향 꼭짓점 찾기
FVector UBTTask_ReturnToSquare::GetCounterClockwiseCorner(const FVector& EdgePoint, const FVector& SquareCenter, float SquareSize) const
{
    FVector Corners[4] = {
        SquareCenter + FVector(-SquareSize, -SquareSize, 0), // Bottom-left
        SquareCenter + FVector(-SquareSize, SquareSize, 0),  // Top-left
        SquareCenter + FVector(SquareSize, SquareSize, 0),   // Top-right
        SquareCenter + FVector(SquareSize, -SquareSize, 0)   // Bottom-right
    };

    // EdgePoint와 꼭짓점 간의 거리를 기반으로 가장 가까운 꼭짓점 찾기
    int32 CurrentCornerIndex = -1;

    if (EdgePoint.Equals(SquareCenter + FVector(-SquareSize, 0, 0), 1.0f)) // Left-center
    {
        CurrentCornerIndex = 0; // Bottom-left
    }
    else if (EdgePoint.Equals(SquareCenter + FVector(0, SquareSize, 0), 1.0f)) // Top-center
    {
        CurrentCornerIndex = 1; // Top-left
    }
    else if (EdgePoint.Equals(SquareCenter + FVector(SquareSize, 0, 0), 1.0f)) // Right-center
    {
        CurrentCornerIndex = 2; // Top-right
    }
    else if (EdgePoint.Equals(SquareCenter + FVector(0, -SquareSize, 0), 1.0f)) // Bottom-center
    {
        CurrentCornerIndex = 3; // Bottom-right
    }

    // 반시계 방향 규칙 적용
    if (CurrentCornerIndex >= 0)
    {
        return Corners[(CurrentCornerIndex + 1) % 4]; // 다음 반시계 방향 꼭짓점 반환                    
    }

    // 만약 EdgePoint가 예상하지 않은 위치라면 기본 거리 계산
    FVector ClosestCorner = Corners[0];
    float ClosestDistance = FVector::Dist(EdgePoint, Corners[0]);

    for (int32 i = 1; i < 4; ++i)
    {
        float Distance = FVector::Dist(EdgePoint, Corners[i]);
        if (Distance < ClosestDistance)
        {
            ClosestCorner = Corners[i];
            ClosestDistance = Distance;
        }
    }

    return ClosestCorner;

   
}       