// Fill out your copyright notice in the Description page of Project Settings.


#include "10_Enemy/BTTask/BTTask_MoveToCornerVent.h"
#include "10_Enemy/BossEnemy.h"
#include "10_Enemy/VentPoint.h"

#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Kismet/GameplayStatics.h"

UBTTask_MoveToCornerVent::UBTTask_MoveToCornerVent()
{
    NodeName = "Move To Corner Vent Instantly";
}

EBTNodeResult::Type UBTTask_MoveToCornerVent::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    // AIController 가져오기
    AAIController* AIController = OwnerComp.GetAIOwner();
    if (!AIController)
    {
        UE_LOG(LogTemp, Error, TEXT("AIController is null in MoveToCornerVent task"));
        return EBTNodeResult::Failed;
    }

    // 보스 캐릭터 가져오기
    ABossEnemy* Boss = Cast<ABossEnemy>(AIController->GetPawn());
    if (!Boss)
    {
        UE_LOG(LogTemp, Error, TEXT("BossEnemy is null in MoveToCornerVent task"));
        return EBTNodeResult::Failed;
    }

    // 블랙보드 가져오기
    UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();
    if (!BlackboardComp)
    {
        UE_LOG(LogTemp, Error, TEXT("BlackboardComponent is null in MoveToCornerVent task"));
        return EBTNodeResult::Failed;
    }

    // 코너 벤트 번호 배열 정의
    //TArray<int32> CornerVentNumbers = { 1, 3, 7, 9 };

    // 랜덤 코너 벤트 번호 선택
    int32 RandomIndex = FMath::RandRange(0, CornerVentNumbers.Num() - 1);
    int32 SelectedVentNumber = CornerVentNumbers[RandomIndex];

    // 선택된 벤트 번호에 해당하는 벤트 위치 찾기
    TArray<AActor*> FoundVents;
    UGameplayStatics::GetAllActorsOfClass(Boss->GetWorld(), AVentPoint::StaticClass(), FoundVents);

    FVector SelectedVentLocation;
    for (AActor* Actor : FoundVents)
    {
        AVentPoint* Vent = Cast<AVentPoint>(Actor);
        if (Vent && Vent->VentNumber == SelectedVentNumber)
        {
            // 벤트의 공중 위치 계산
            SelectedVentLocation = Vent->GetVentLocation() + FVector(0, 0, 1000.0f); // 공중으로 1000 유닛 이동
            break;
        }
    }

    // 선택된 벤트 위치가 유효하지 않은 경우 실패
    if (SelectedVentLocation.IsZero())
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to find corner vent with number: %d"), SelectedVentNumber);
        return EBTNodeResult::Failed;
    }

    // 보스를 선택된 위치로 즉시 이동
    Boss->SetActorLocation(SelectedVentLocation);

    // 블랙보드에 선택된 벤트 번호 저장
    BlackboardComp->SetValueAsInt(GetSelectedBlackboardKey(), SelectedVentNumber);

    UE_LOG(LogTemp, Log, TEXT("Boss moved instantly to Corner Vent #%d at location: %s"), SelectedVentNumber, *SelectedVentLocation.ToString());

    return EBTNodeResult::Succeeded;
}

