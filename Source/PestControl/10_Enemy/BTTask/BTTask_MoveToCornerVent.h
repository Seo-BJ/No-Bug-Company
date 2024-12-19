// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "BTTask_MoveToCornerVent.generated.h"

/**
 * 
 */
UCLASS()
class PESTCONTROL_API UBTTask_MoveToCornerVent : public UBTTask_BlackboardBase
{
	GENERATED_BODY()

public:
    UBTTask_MoveToCornerVent();

protected:
    /** Override the ExecuteTask method */
    virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

private:
    /** List of corner vent numbers */
    TArray<int32> CornerVentNumbers = { 1, 3, 7, 9 };
	
};
