// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTDecorator.h"
#include "BTDecorator_ExecutionChance.generated.h"

/**
 * 
 */
UCLASS()
class PESTCONTROL_API UBTDecorator_ExecutionChance : public UBTDecorator
{
	GENERATED_BODY()

public:
    UBTDecorator_ExecutionChance();

protected:
    /** Probability of execution (0.0 to 1.0) */
    UPROPERTY(EditAnywhere, Category = "Chance")
    float ExecutionChance;

    /** Called when the decorator checks if it should allow task execution */
    virtual bool CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const override;
	
};
