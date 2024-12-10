// Fill out your copyright notice in the Description page of Project Settings.


#include "10_Enemy/BTTask_MoveToVent.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "10_Enemy/BossEnemy.h"
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

    // 랜덤 환풍구 위치 선택
    if (Boss->VentLocations.Num() == 0)
    {
        UE_LOG(LogTemp, Error, TEXT("No vent locations set for BossEnemy"));
        return EBTNodeResult::Failed;
    }

    int32 RandomIndex = FMath::RandRange(0, Boss->VentLocations.Num() - 1);
    FVector TargetVent = Boss->VentLocations[RandomIndex];
    FVector VentAboveLocation = TargetVent + FVector(0, 0, 1000.0f); // 환풍구 위 하늘

    // 보스 위치를 환풍구 위로 즉시 이동
    Boss->SetActorLocation(VentAboveLocation);

    // 블랙보드에 이동할 위치 저장
    UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();
    if (BlackboardComp)
    {
        BlackboardComp->SetValueAsVector(GetSelectedBlackboardKey(), VentAboveLocation);
    }

    // **플레이어를 바라보도록 회전 설정**
    APeCoPlayerCharacter* PlayerCharacter = Cast<APeCoPlayerCharacter>(UGameplayStatics::GetPlayerCharacter(Boss->GetWorld(), 0));
    if (PlayerCharacter)
    {
        FVector ToPlayer = PlayerCharacter->GetActorLocation() - Boss->GetActorLocation();
        FRotator NewRotation = ToPlayer.Rotation();
        Boss->SetActorRotation(FRotator(0.f, NewRotation.Yaw, 0.f)); // Yaw만 회전
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("PlayerCharacter is not of type APeCoPlayerCharacter"));
    }

    UE_LOG(LogTemp, Log, TEXT("Boss moved instantly to vent above: %s"), *VentAboveLocation.ToString());
    return EBTNodeResult::Succeeded;
}
