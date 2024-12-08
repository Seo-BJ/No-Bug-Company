// Fill out your copyright notice in the Description page of Project Settings.


#include "10_Enemy/BTTask_MoveCounterClockwise.h"

#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "GameFramework/Pawn.h"

UBTTask_MoveCounterClockwise::UBTTask_MoveCounterClockwise()
{
    NodeName = "Move Counter Clockwise";
}

EBTNodeResult::Type UBTTask_MoveCounterClockwise::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    AAIController* AIController = OwnerComp.GetAIOwner();
    if (!AIController)
    {
        UE_LOG(LogTemp, Error, TEXT("AIController is null"));
        return EBTNodeResult::Failed;
    }

    APawn* ControlledPawn = AIController->GetPawn();
    if (!ControlledPawn)
    {
        UE_LOG(LogTemp, Error, TEXT("Controlled Pawn is null"));
        return EBTNodeResult::Failed;
    }

    FVector CurrentLocation = ControlledPawn->GetActorLocation();

    // 다음 반시계 방향 위치 계산
    FVector NextLocation = GetNextCounterClockwisePoint(CurrentLocation);

    // 블랙보드에 다음 위치 저장
    UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();
    if (BlackboardComp)
    {
        BlackboardComp->SetValueAsVector(GetSelectedBlackboardKey(), NextLocation);
    }

    return EBTNodeResult::Succeeded;
}

FVector UBTTask_MoveCounterClockwise::GetNextCounterClockwisePoint(const FVector& CurrentLocation) const
{
    // 정사각형 테두리의 좌표와 현재 위치를 기반으로 다음 이동 지점 계산
    FVector SquareCenter(1390.f, -160.f, CurrentLocation.Z); // 정사각형 중심
    float SquareSize = 1000.f;                         // 정사각형 한 변 길이
    FVector Points[4] = {
        SquareCenter + FVector(-SquareSize, -SquareSize, 0), // Bottom-left
        SquareCenter + FVector(-SquareSize, SquareSize, 0),  // Top-left
        SquareCenter + FVector(SquareSize, SquareSize, 0),   // Top-right
        SquareCenter + FVector(SquareSize, -SquareSize, 0)   // Bottom-right
    };


    // 현재 위치를 정사각형 테두리의 가장 가까운 지점으로 스냅
    FVector SnappedLocation = CurrentLocation;

    // X 좌표 스냅
    if (FMath::Abs(CurrentLocation.X - (SquareCenter.X - SquareSize)) < 1.f)
    {
        SnappedLocation.X = SquareCenter.X - SquareSize; // Left edge
    }
    else if (FMath::Abs(CurrentLocation.X - (SquareCenter.X + SquareSize)) < 1.f)
    {
        SnappedLocation.X = SquareCenter.X + SquareSize; // Right edge
    }

    // Y 좌표 스냅
    if (FMath::Abs(CurrentLocation.Y - (SquareCenter.Y - SquareSize)) < 1.f)
    {
        SnappedLocation.Y = SquareCenter.Y - SquareSize; // Bottom edge
    }
    else if (FMath::Abs(CurrentLocation.Y - (SquareCenter.Y + SquareSize)) < 1.f)
    {
        SnappedLocation.Y = SquareCenter.Y + SquareSize; // Top edge
    }

    // 가장 가까운 꼭짓점 찾기
    int32 ClosestIndex = 0;
    float ClosestDistance = FVector::Dist(SnappedLocation, Points[0]);
    for (int32 i = 1; i < 4; ++i)
    {
        float Distance = FVector::Dist(SnappedLocation, Points[i]);
        if (Distance < ClosestDistance)
        {
            ClosestDistance = Distance;
            ClosestIndex = i;
        }
    }

    // 다음 반시계 방향 위치 반환
    return Points[(ClosestIndex + 1) % 4];    

}

