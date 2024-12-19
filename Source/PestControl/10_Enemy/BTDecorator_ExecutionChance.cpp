// Fill out your copyright notice in the Description page of Project Settings.


#include "10_Enemy/BTDecorator_ExecutionChance.h"

#include "BehaviorTree/BlackboardComponent.h"
#include "Math/UnrealMathUtility.h"

UBTDecorator_ExecutionChance::UBTDecorator_ExecutionChance()
{
    NodeName = "Execution Chance";
    ExecutionChance = 0.5f; // 기본 확률 50%
}

bool UBTDecorator_ExecutionChance::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
    float RandomValue = FMath::FRand(); // 0.0 ~ 1.0 사이의 랜덤 값 생성
    return RandomValue <= ExecutionChance;
}

