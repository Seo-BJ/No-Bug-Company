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

	HeightAdjustmentThreshold = 200.0f; // Z 축 이동 보정 값

	bNotifyTick = true; // Enable TickTask

}

EBTNodeResult::Type UBTTask_FlyTo::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	// AIController 및 제어 중인 캐릭터 가져오기
	AAIController* AIController = OwnerComp.GetAIOwner();
	ACharacter* ControlledCharacter = Cast<ACharacter>(AIController->GetPawn());

	if (!ControlledCharacter)
	{
		return EBTNodeResult::Failed;
	}

	return EBTNodeResult::InProgress;
}

void UBTTask_FlyTo::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	// AIController 및 제어 중인 캐릭터 가져오기
	AAIController* AIController = OwnerComp.GetAIOwner();
	ACharacter* ControlledCharacter = Cast<ACharacter>(AIController->GetPawn());

	if (!ControlledCharacter)
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
		return;
	}

	// Blackboard에서 플레이어 위치 가져오기
	FVector PlayerLocation = OwnerComp.GetBlackboardComponent()->GetValueAsVector("PlayerLocation");
	FVector CurrentLocation = ControlledCharacter->GetActorLocation();

	// Z 축 높이 조정
	if (FMath::Abs(CurrentLocation.Z - PlayerLocation.Z) > HeightAdjustmentThreshold)
	{
		// 목표 위치의 높이를 AI 캐릭터의 위치에 맞게 조정
		PlayerLocation.Z = CurrentLocation.Z + (PlayerLocation.Z > CurrentLocation.Z ? HeightAdjustmentThreshold : -HeightAdjustmentThreshold);
	}

	// 목표를 향해 이동
	FVector Direction = (PlayerLocation - CurrentLocation).GetSafeNormal();
	ControlledCharacter->AddMovementInput(Direction);

	// 목표에 도달했는지 확인
	if (FVector::Dist(CurrentLocation, PlayerLocation) <= AcceptanceRadius)
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
	}
}

