// Fill out your copyright notice in the Description page of Project Settings.


#include "10_Enemy/BTTask/BTTask_MoveCounterClockwise.h"

#include "10_Enemy/BossEnemy.h"

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

    ABossEnemy* Boss = Cast<ABossEnemy>(AIController->GetPawn());
    if (!Boss)
    {
        UE_LOG(LogTemp, Warning, TEXT("Boss is null in BTTask_MoveCounterClockwise"));
        return EBTNodeResult::Failed;
    }

    FVector CurrentLocation = Boss->GetActorLocation();
    FVector CenterVentLocation = Boss->CenterVentLocation;

    // 중앙 벤트 위치 유효성 검사
    if (CenterVentLocation.IsZero())
    {
        UE_LOG(LogTemp, Warning, TEXT("CenterVentLocation is invalid"));
        return EBTNodeResult::Failed;
    }
        
    // Boss의 SquareSize 가져오기
    float SquareSize = Boss->SquareSize;   
    
    // 다음 반시계 방향 위치 계산
    FVector NextLocation = GetNextCounterClockwisePoint(CurrentLocation, CenterVentLocation, SquareSize);

    // **디버그 로그 추가**    
    UE_LOG(LogTemp, Log, TEXT("Boss Current Location: %s"), *CurrentLocation.ToString());             
    UE_LOG(LogTemp, Log, TEXT("Boss Target Location (NextLocation): %s"), *NextLocation.ToString());    


     

    // 블랙보드에 다음 위치 저장
    UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();   
    if (BlackboardComp)
    {
        BlackboardComp->SetValueAsVector(GetSelectedBlackboardKey(), NextLocation);
    }

    return EBTNodeResult::Succeeded;                                                 
}

FVector UBTTask_MoveCounterClockwise::GetNextCounterClockwisePoint(const FVector& CurrentLocation, const FVector& SquareCenter ,float SquareSize) const    
{
         
    FVector Points[4] = {
        SquareCenter + FVector(-SquareSize, -SquareSize, 0), // Bottom-left
        SquareCenter + FVector(-SquareSize, SquareSize, 0),  // Top-left
        SquareCenter + FVector(SquareSize, SquareSize, 0),   // Top-right
        SquareCenter + FVector(SquareSize, -SquareSize, 0)   // Bottom-right      
    };
      

    // 현재 위치와 가장 가까운 꼭짓점 찾기
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

    // 다음 반시계 방향 위치 반환
    return Points[(ClosestIndex + 1) % 4];

}

