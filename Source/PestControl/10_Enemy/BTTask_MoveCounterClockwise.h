// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "BTTask_MoveCounterClockwise.generated.h"

/**
 * 
 */
UCLASS()
class PESTCONTROL_API UBTTask_MoveCounterClockwise : public UBTTask_BlackboardBase
{
	GENERATED_BODY()

public:
    UBTTask_MoveCounterClockwise();

protected:
    virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

private:
    // 다음 이동 지점 계산
    FVector GetNextCounterClockwisePoint(const FVector& CurrentLocation) const;
	
};
