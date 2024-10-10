// Fill out your copyright notice in the Description page of Project Settings.


#include "PeCoEnemyAIController.h"

#include "Kismet/GameplayStatics.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/BehaviorTree.h"




void APeCoEnemyAIController::BeginPlay()
{
	Super::BeginPlay();

	// AIBehavior와 BlackboardComponent가 설정되어 있는지 확인
	if (!AIBehavior || !UseBlackboard(AIBehavior->BlackboardAsset, BlackboardComponent))
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to initialize AI in %s"), *GetName());
		return;
	}

	// Behavior Tree 실행
	if (!RunBehaviorTree(AIBehavior))
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to run Behavior Tree for %s"), *GetName());
		return;
	}

	// Pawn이 올바르게 설정되었는지 확인 후 시작 위치를 설정
	APawn* ControlledPawn = GetPawn();
	if (ControlledPawn && BlackboardComponent)
	{
		FVector StartLocation = ControlledPawn->GetActorLocation();
		BlackboardComponent->SetValueAsVector(TEXT("StartLocation"), StartLocation);
	}
			
}

void APeCoEnemyAIController::Tick(float DeltaSeconds) 
{
	Super::Tick(DeltaSeconds);

	APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
	if (!PlayerPawn || !GetBlackboardComponent())
	{
		return;
	}
	

	if (LineOfSightTo(PlayerPawn))
	{
		GetBlackboardComponent()->SetValueAsVector(TEXT("PlayerLocation"), PlayerPawn->GetActorLocation());
		GetBlackboardComponent()->SetValueAsVector(TEXT("LastKnownPlayerLocation"), PlayerPawn->GetActorLocation());
	}
	else 
	{
		GetBlackboardComponent()-> ClearValue(TEXT("PlayerLocation"));
	}

}
