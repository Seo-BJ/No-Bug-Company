// Fill out your copyright notice in the Description page of Project Settings.


#include "10_Enemy/PeCoGroundEnemyAIController.h"

#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/BehaviorTree.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Actor.h"

void APeCoGroundEnemyAIController::BeginPlay()
{
	Super::BeginPlay();

	if (GroundAIBehavior != nullptr)
	{
		// Initialize Blackboard
		if (UseBlackboard(GroundAIBehavior->BlackboardAsset, BlackboardComponent))
		{
			// Start the behavior tree
			RunBehaviorTree(GroundAIBehavior);
			/*APawn* ControlledPawn = GetPawn();
			if (ControlledPawn && BlackboardComponent)
			{
				FVector StartLocation = ControlledPawn->GetActorLocation();
				BlackboardComponent->SetValueAsVector(TEXT("StartLocation"), StartLocation);
				
			}*/
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("Failed to initialize Blackboard for %s"), *GetName());
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("GroundAIBehavior is null in %s"), *GetName());
	}
}

void APeCoGroundEnemyAIController::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (bIsKnockedBack) // 넉백 상태라면 이동 중지
	{
		// 타이머 클리어 및 상태 리셋
		GetWorld()->GetTimerManager().ClearTimer(KnockbackTimerHandle);
		return;
	}

	APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
	
	if (PlayerPawn && BlackboardComponent) // Ensure BlackboardComponent is valid
	{
		if (LineOfSightTo(PlayerPawn)) // Check if the player is within sight
		{
			BlackboardComponent->SetValueAsVector(TEXT("PlayerLocation"), PlayerPawn->GetActorLocation());
			GetBlackboardComponent()->SetValueAsVector(TEXT("LastKnownPlayerLocation"), PlayerPawn->GetActorLocation());
		}
		else
		{
			BlackboardComponent->ClearValue(TEXT("PlayerLocation"));
		}
	}
	
}

void APeCoGroundEnemyAIController::SetIsKnockedBack(bool IsKnockedBack)
{
	bIsKnockedBack = IsKnockedBack;
	if (BlackboardComponent)
	{
		BlackboardComponent->SetValueAsBool(TEXT("bIsKnockedBack"), IsKnockedBack);
	}
}