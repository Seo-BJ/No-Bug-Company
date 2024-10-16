// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "PeCoFlyingEnemyAIController.generated.h"

/**
 * 
 */
UCLASS()
class PESTCONTROL_API APeCoFlyingEnemyAIController : public AAIController
{
	GENERATED_BODY()
	
protected:
    virtual void BeginPlay() override;

public:
    virtual void Tick(float DeltaSeconds) override;

    
    // Behavior Tree
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
    UBehaviorTree* FlyingAIBehavior;

    // Blackboard Component 
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI")
    UBlackboardComponent* BlackboardComponent;

};
