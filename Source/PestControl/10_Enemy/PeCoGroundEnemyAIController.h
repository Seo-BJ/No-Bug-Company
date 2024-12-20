// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "PeCoGroundEnemyAIController.generated.h"

/**
 * 
 */
UCLASS()
class PESTCONTROL_API APeCoGroundEnemyAIController : public AAIController
{
	GENERATED_BODY()

public:

	virtual void Tick(float DeltaSeconds) override;
	
	void SetIsKnockedBack(bool IsKnockedBack);
	bool IsKnockedBack() const { return bIsKnockedBack; }

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	
	void ResetKnockback();
	
protected:
	virtual void BeginPlay() override;

	// Behavior Tree asset
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	UBehaviorTree* GroundAIBehavior;

	// Blackboard Component 
	UPROPERTY(BlueprintReadWrite, Category = "AI")
	UBlackboardComponent* BlackboardComponent;

private:
	bool bIsKnockedBack = false;
	
	// Timer handle for managing knockback duration
	FTimerHandle KnockbackTimerHandle;
};
