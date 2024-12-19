// Fill out your copyright notice in the Description page of Project Settings.


#include "10_Enemy/BTTask/BTTask_MoveToVent.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "10_Enemy/BossEnemy.h"
#include "10_Enemy/VentPoint.h"
#include "01_Character/PeCoPlayerCharacter.h" 
#include "Kismet/GameplayStatics.h"

UBTTask_MoveToVent::UBTTask_MoveToVent()
{
    NodeName = "Move To Vent Instantly";
}

EBTNodeResult::Type UBTTask_MoveToVent::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    AAIController* AIController = OwnerComp.GetAIOwner();
    if (!AIController)
    {
        UE_LOG(LogTemp, Error, TEXT("AIController is null in MoveToVent task"));
        return EBTNodeResult::Failed;
    }

    ABossEnemy* Boss = Cast<ABossEnemy>(AIController->GetPawn());
    if (!Boss)
    {
        UE_LOG(LogTemp, Error, TEXT("BossEnemy is null in MoveToVent task"));
        return EBTNodeResult::Failed;
    }

    // 사용 가능한 벤트 번호가 없는 경우 모든 벤트를 다시 초기화
    if (Boss->AvailableVentIndices.Num() == 0)
    {
        Boss->ResetVentIndices();
    }

    // 랜덤으로 벤트 선택
    int32 RandomIndex = FMath::RandRange(0, Boss->AvailableVentIndices.Num() - 1);
    int32 SelectedVentNumber = Boss->AvailableVentIndices[RandomIndex];
    Boss->AvailableVentIndices.RemoveAt(RandomIndex); // 선택된 벤트 제거

    // 선택된 벤트의 위치 검색
    TArray<AActor*> FoundVents;
    UGameplayStatics::GetAllActorsOfClass(Boss->GetWorld(), AVentPoint::StaticClass(), FoundVents);

    for (AActor* Actor : FoundVents)
    {
        AVentPoint* Vent = Cast<AVentPoint>(Actor);
        if (Vent && Vent->VentNumber == SelectedVentNumber)
        {
            FVector VentAboveLocation = Vent->GetActorLocation() + FVector(0, 0, 1000.0f); // 환풍구 위 하늘 위치

            // 보스를 해당 위치로 즉시 이동
            Boss->SetActorLocation(VentAboveLocation);

            // 블랙보드에 이동한 위치 저장
            UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();
            if (BlackboardComp)
            {
                BlackboardComp->SetValueAsVector(GetSelectedBlackboardKey(), VentAboveLocation);
            }

            // 플레이어를 바라보도록 회전 설정
            APeCoPlayerCharacter* PlayerCharacter = Cast<APeCoPlayerCharacter>(UGameplayStatics::GetPlayerCharacter(Boss->GetWorld(), 0));
            if (PlayerCharacter)
            {
                FVector ToPlayer = PlayerCharacter->GetActorLocation() - Boss->GetActorLocation();
                FRotator NewRotation = ToPlayer.Rotation();
                Boss->SetActorRotation(FRotator(0.f, NewRotation.Yaw, 0.f)); // Yaw만 회전
            }

            UE_LOG(LogTemp, Log, TEXT("Boss moved to Vent #%d at location: %s"), Vent->VentNumber, *VentAboveLocation.ToString());
            return EBTNodeResult::Succeeded;
        }
    }

    UE_LOG(LogTemp, Error, TEXT("Failed to find vent for Boss"));
    return EBTNodeResult::Failed;

    
}
