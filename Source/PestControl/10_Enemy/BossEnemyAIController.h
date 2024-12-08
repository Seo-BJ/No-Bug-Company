// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BossEnemyAIController.generated.h"

/**
 * 
 */
UCLASS()
class PESTCONTROL_API ABossEnemyAIController : public AAIController
{
	GENERATED_BODY()
	
public:
    ABossEnemyAIController();

    // BeginPlay에서 Behavior Tree를 초기화
    virtual void BeginPlay() override;

    // Behavior Tree를 실행하는 함수
    void StartBehaviorTree();

    // 블랙보드 접근자
    UBlackboardComponent* GetBlackboardComponent() const;

    // 보스의 행동을 중지
    void StopLogic(const FString& Reason);

protected:
    // 블랙보드 컴포넌트
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI")
    UBlackboardComponent* BlackboardComponent;

    // Behavior Tree Asset
    UPROPERTY(EditDefaultsOnly, Category = "AI")
    UBehaviorTree* BossAIBehavior;

    // 초기 블랙보드 값 설정
    virtual void InitializeBlackboardValues();

};
