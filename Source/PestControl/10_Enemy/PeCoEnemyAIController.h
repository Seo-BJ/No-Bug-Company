// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "PeCoEnemyAIController.generated.h"

/**
 * 
 */
UCLASS()
class PESTCONTROL_API APeCoEnemyAIController : public AAIController
{
	GENERATED_BODY()

public:
	virtual void Tick(float DeltaSeconds) override;
protected:
	virtual void BeginPlay() override;

	// Behavior Tree asset
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	UBehaviorTree* AIBehavior;

	// Blackboard Component 
	UPROPERTY(BlueprintReadWrite, Category = "AI")
	UBlackboardComponent* BlackboardComponent;

};
