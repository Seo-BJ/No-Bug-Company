// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "BTTask_ReturnToSquare.generated.h"

/**
 * 
 */
UCLASS()
class PESTCONTROL_API UBTTask_ReturnToSquare : public UBTTask_BlackboardBase
{
	GENERATED_BODY()

public:
    UBTTask_ReturnToSquare();

protected:
    virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

private:
    // 원래 이동 공간의 중심과 크기
    FVector SquareCenter = FVector(1390.f, -160.f, 1180.f); // 정사각형 공간 중심
    float SquareSize = 1000.f; // 정사각형 한 변의 길이

    // 가장 가까운 테두리 지점 계산
    FVector GetClosestEdgePoint(const FVector& CurrentLocation) const;
	
};
