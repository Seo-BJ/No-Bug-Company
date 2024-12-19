// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "BTTask_RushAttack.generated.h"

/**
 * 
 */
UCLASS()
class PESTCONTROL_API UBTTask_RushAttack : public UBTTask_BlackboardBase
{
	GENERATED_BODY()

public:
	UBTTask_RushAttack();

protected:
	/** Override the ExecuteTask method */
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	
};
