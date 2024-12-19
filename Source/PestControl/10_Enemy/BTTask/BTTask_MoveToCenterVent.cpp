// Fill out your copyright notice in the Description page of Project Settings.


#include "10_Enemy/BTTask/BTTask_MoveToCenterVent.h"

#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "10_Enemy/BossEnemy.h"
#include "01_Character/PeCoPlayerCharacter.h"
#include "Kismet/GameplayStatics.h"

UBTTask_MoveToCenterVent::UBTTask_MoveToCenterVent()
{
    NodeName = "Move To Center Vent Instantly";
}

EBTNodeResult::Type UBTTask_MoveToCenterVent::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    AAIController* AIController = OwnerComp.GetAIOwner();
    if (!AIController)
    {
        UE_LOG(LogTemp, Error, TEXT("AIController is null in MoveToCenterVent task"));
        return EBTNodeResult::Failed;
    }

    ABossEnemy* Boss = Cast<ABossEnemy>(AIController->GetPawn());
    if (!Boss)
    {
        UE_LOG(LogTemp, Error, TEXT("BossEnemy is null in MoveToCenterVent task"));
        return EBTNodeResult::Failed;
    }

    // 중앙 벤트 위치 가져오기
    FVector CenterVentLocation = Boss->CenterVentLocation;
    if (CenterVentLocation.IsZero())
    {
        UE_LOG(LogTemp, Error, TEXT("CenterVentLocation is invalid"));
        return EBTNodeResult::Failed;
    }

    // 보스를 중앙 벤트 위로 이동
    FVector AboveCenterVentLocation = CenterVentLocation + FVector(0, 0, 1000.0f); // 중앙 벤트 위 하늘 위치
    Boss->SetActorLocation(AboveCenterVentLocation);

    // 블랙보드에 중앙 벤트 위치 저장
    UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();
    if (BlackboardComp)
    {
        BlackboardComp->SetValueAsVector(GetSelectedBlackboardKey(), AboveCenterVentLocation);
    }

    // 플레이어를 바라보도록 회전 설정
    APeCoPlayerCharacter* PlayerCharacter = Cast<APeCoPlayerCharacter>(UGameplayStatics::GetPlayerCharacter(Boss->GetWorld(), 0));
    if (PlayerCharacter)
    {
        FVector ToPlayer = PlayerCharacter->GetActorLocation() - Boss->GetActorLocation();
        FRotator NewRotation = ToPlayer.Rotation();
        Boss->SetActorRotation(FRotator(0.f, NewRotation.Yaw, 0.f)); // Yaw만 회전
    }

    UE_LOG(LogTemp, Log, TEXT("Boss moved to Center Vent at location: %s"), *AboveCenterVentLocation.ToString());
    return EBTNodeResult::Succeeded;
}

