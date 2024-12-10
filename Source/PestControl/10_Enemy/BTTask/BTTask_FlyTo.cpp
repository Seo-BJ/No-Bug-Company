// Fill out your copyright notice in the Description page of Project Settings.


#include "10_Enemy/BTTask/BTTask_FlyTo.h"
#include "10_Enemy/PeCoFlyingEnemyCharacter.h"

#include "BehaviorTree/BlackboardComponent.h"
#include "AIController.h"

#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"

UBTTask_FlyTo::UBTTask_FlyTo()
{
	NodeName = "Fly To Player";

	AcceptanceRadius = 50.0f; // Default value: 50 units

	bNotifyTick = true; // Enable TickTask

}
EBTNodeResult::Type UBTTask_FlyTo::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* AIController = OwnerComp.GetAIOwner();
	ACharacter* ControlledCharacter = Cast<ACharacter>(AIController->GetPawn()); 

	if (ControlledCharacter)
	{
		FVector PlayerLocation = OwnerComp.GetBlackboardComponent()->GetValueAsVector("PlayerLocation");

		// Get flying speed from the flying enemy character
		APeCoFlyingEnemyCharacter * FlyingEnemy = Cast<APeCoFlyingEnemyCharacter>(ControlledCharacter);
		float Speed = FlyingEnemy ? FlyingEnemy->FlyingSpeed : 1.0f;  // Use FlyingSpeed, default to 1.0f if not available
		
		// Measure the distance to the player and detect obstacles
		FHitResult HitResult;
		FVector Start = ControlledCharacter->GetActorLocation();
		FVector End = PlayerLocation;

		// Use line trace to detect obstacles
		bool bHitObstacle = GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECC_Visibility);

		if (bHitObstacle)
		{
			// Adjust Z-axis to move above obstacles (raise by 400 units)
			End.Z += 400.0f;
		}

		// Perform flying movement
		FVector Direction = (End - Start).GetSafeNormal();
		ControlledCharacter->AddMovementInput(Direction, Speed / 600.0f);
				
		return EBTNodeResult::InProgress;
	}

	return EBTNodeResult::Failed;
}

void UBTTask_FlyTo::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	AAIController* AIController = OwnerComp.GetAIOwner();
	ACharacter* ControlledCharacter = Cast<ACharacter>(AIController->GetPawn());


	if (ControlledCharacter)
	{
		// Get the player's location from the blackboard
		FVector PlayerLocation = OwnerComp.GetBlackboardComponent()->GetValueAsVector("PlayerLocation");
		FVector Start = ControlledCharacter->GetActorLocation();
		
		// Detect obstacles
		FHitResult HitResult;
		bool bHitObstacle = GetWorld()->LineTraceSingleByChannel(HitResult, Start, PlayerLocation, ECC_Visibility);

		if (bHitObstacle)
		{
			PlayerLocation.Z += 400.0f;  // Raise the player location by 400 units to move above obstacles
		}

		// Apply flying speed and direction to move
		FVector Direction = (PlayerLocation - Start).GetSafeNormal();

		// Get flying speed from the flying enemy character
		APeCoFlyingEnemyCharacter* FlyingEnemy = Cast<APeCoFlyingEnemyCharacter>(ControlledCharacter);
		float Speed = FlyingEnemy ? FlyingEnemy->FlyingSpeed : 1.0f;

		// Apply speed and fly towards the player
		ControlledCharacter->AddMovementInput(Direction, Speed / 600.0f);
				
		// Check if the target point has been reached
		if (FVector::Dist(ControlledCharacter->GetActorLocation(), PlayerLocation) <= AcceptanceRadius)
		{
			FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);			
		}		
	}
}