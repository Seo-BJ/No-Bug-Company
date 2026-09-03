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
	APeCoEnemyAIController();
	virtual void Tick(float DeltaSeconds) override;

	// Enemy 풀 반환/재획득 시 Possess 관계는 유지하고 AI 실행 상태만 전환한다.
	void SuspendForPooling();
	void ResumeFromPooling();
protected:
	virtual void BeginPlay() override;

	// Behavior Tree asset
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	UBehaviorTree* AIBehavior;

	// Blackboard Component 
	UPROPERTY(BlueprintReadWrite, Category = "AI")
	UBlackboardComponent* BlackboardComponent;

private:
	void ResetBlackboardForCurrentPawn();

	bool bSuspendedForPooling = false;
};
