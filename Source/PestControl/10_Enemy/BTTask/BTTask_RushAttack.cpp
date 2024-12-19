// Fill out your copyright notice in the Description page of Project Settings.


#include "10_Enemy/BTTask/BTTask_RushAttack.h"

#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "10_Enemy/BossEnemy.h"

UBTTask_RushAttack::UBTTask_RushAttack()
{
    NodeName = "Perform Rush Attack";
}

EBTNodeResult::Type UBTTask_RushAttack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    // AIController 가져오기
    AAIController* AIController = OwnerComp.GetAIOwner();
    if (!AIController)
    {
        UE_LOG(LogTemp, Error, TEXT("AIController is null in PerformRushAttack task"));
        return EBTNodeResult::Failed;
    }

    // 보스 캐릭터 가져오기
    ABossEnemy* Boss = Cast<ABossEnemy>(AIController->GetPawn());
    if (!Boss)
    {
        UE_LOG(LogTemp, Error, TEXT("BossEnemy is null in PerformRushAttack task"));
        return EBTNodeResult::Failed;
    }

    // 블랙보드에서 현재 벤트 번호 가져오기
    UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();
    if (!BlackboardComp)
    {
        UE_LOG(LogTemp, Error, TEXT("BlackboardComponent is null in PerformRushAttack task"));
        return EBTNodeResult::Failed;
    }

    int32 CurrentVentNumber = BlackboardComp->GetValueAsInt(GetSelectedBlackboardKey());

    // 보스의 돌진 공격 함수 호출
    Boss->PerformRushAttackFromVent(CurrentVentNumber);

    // 작업 성공 반환
    return EBTNodeResult::Succeeded;
}