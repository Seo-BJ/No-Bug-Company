// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "BTTask_AOEAttack.generated.h"

/**
 * 
 */
UCLASS()
class PESTCONTROL_API UBTTask_AOEAttack : public UBTTask_BlackboardBase
{
	GENERATED_BODY()

public:
	UBTTask_AOEAttack();

protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	
};
