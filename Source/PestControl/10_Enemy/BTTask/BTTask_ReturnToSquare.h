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
    
    // 가장 가까운 테두리 지점 계산
    FVector GetClosestEdgePoint(const FVector& CurrentLocation, const FVector& SquareCenter, float SquareSize) const;                

    FVector GetCounterClockwiseCorner(const FVector& EdgePoint, const FVector& SquareCenter, float SquareSize) const; 
	
};
