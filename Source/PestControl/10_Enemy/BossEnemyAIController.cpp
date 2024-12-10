// Fill out your copyright notice in the Description page of Project Settings.


#include "10_Enemy/BossEnemyAIController.h"
#include "10_Enemy/BossEnemy.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Kismet/GameplayStatics.h"

ABossEnemyAIController::ABossEnemyAIController()
{
    // 블랙보드 컴포넌트 생성
    BlackboardComponent = CreateDefaultSubobject<UBlackboardComponent>(TEXT("BlackboardComponent"));
}

void ABossEnemyAIController::BeginPlay()
{
    Super::BeginPlay();

    // 보스에 연결된 Behavior Tree를 실행
    StartBehaviorTree();
}

void ABossEnemyAIController::StartBehaviorTree()
{
    if (BossAIBehavior)
    {
        // 보스 캐릭터 가져오기
        ABossEnemy* BossEnemy = Cast<ABossEnemy>(GetPawn());
        if (BossEnemy)
        {
            // 블랙보드 초기화
            if (BlackboardComponent && BossAIBehavior->BlackboardAsset)
            {
                BlackboardComponent->InitializeBlackboard(*(BossAIBehavior->BlackboardAsset));
                InitializeBlackboardValues(); // 블랙보드 기본 값 설정
            }

            // Behavior Tree 실행
            RunBehaviorTree(BossAIBehavior);
        }
        else
        {
            UE_LOG(LogTemp, Warning, TEXT("Failed to cast Pawn to BossEnemy in AIController"));
        }
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("BehaviorTreeAsset is not set in AIController"));
    }
}

void ABossEnemyAIController::InitializeBlackboardValues()
{
    // 초기 블랙보드 값 설정
    if (BlackboardComponent)
    {
        BlackboardComponent->SetValueAsInt(TEXT("Phase"), 1); // 기본 페이즈는 1
        BlackboardComponent->SetValueAsBool(TEXT("IsActive"), true);
    }
}

void ABossEnemyAIController::StopLogic(const FString& Reason)
{
    UE_LOG(LogTemp, Warning, TEXT("Stopping AI Logic: %s"), *Reason);
    BrainComponent->StopLogic(Reason);
}

UBlackboardComponent* ABossEnemyAIController::GetBlackboardComponent() const
{
    return BlackboardComponent;
}