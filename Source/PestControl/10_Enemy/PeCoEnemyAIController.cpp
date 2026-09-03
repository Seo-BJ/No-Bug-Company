// Fill out your copyright notice in the Description page of Project Settings.


#include "PeCoEnemyAIController.h"

#include "Kismet/GameplayStatics.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BrainComponent.h"
#include "Navigation/PathFollowingComponent.h"


APeCoEnemyAIController::APeCoEnemyAIController()
{
	PrimaryActorTick.bCanEverTick = true;
	// 풀링 사용/미사용 모두 엔진 기본 TickGroup(TG_PrePhysics)을 그대로 사용.
}

void APeCoEnemyAIController::BeginPlay()
{
	Super::BeginPlay();

	// AIBehavior�� BlackboardComponent�� �����Ǿ� �ִ��� Ȯ��
	if (!AIBehavior || !UseBlackboard(AIBehavior->BlackboardAsset, BlackboardComponent))
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to initialize AI in %s"), *GetName());
		return;
	}

	// Behavior Tree ����
	if (!RunBehaviorTree(AIBehavior))
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to run Behavior Tree for %s"), *GetName());
		return;
	}

	ResetBlackboardForCurrentPawn();

}

void APeCoEnemyAIController::SuspendForPooling()
{
	if (bSuspendedForPooling)
	{
		return;
	}
	bSuspendedForPooling = true;

	StopMovement();
	ClearFocus(EAIFocusPriority::Gameplay);

	if (UBrainComponent* Brain = GetBrainComponent())
	{
		// StopLogic()은 Behavior Tree를 Safe 모드로 종료하므로 latent abort가 남아
		// 풀 반환 뒤 Tick을 다시 예약할 수 있다. 풀 대기는 즉시 완전히 멈춰야 한다.
		if (UBehaviorTreeComponent* BehaviorTree = Cast<UBehaviorTreeComponent>(Brain))
		{
			BehaviorTree->StopTree(EBTStopMode::Forced);
		}
		else
		{
			Brain->StopLogic(TEXT("Enemy returned to pool"));
		}
		Brain->SetComponentTickEnabled(false);
	}
	if (UPathFollowingComponent* PathFollowing = GetPathFollowingComponent())
	{
		PathFollowing->SetComponentTickEnabled(false);
	}

	SetActorTickEnabled(false);
}

void APeCoEnemyAIController::ResumeFromPooling()
{
	ResetBlackboardForCurrentPawn();

	if (!bSuspendedForPooling)
	{
		return;
	}

	SetActorTickEnabled(true);

	if (UPathFollowingComponent* PathFollowing = GetPathFollowingComponent())
	{
		PathFollowing->SetComponentTickEnabled(true);
	}
	if (UBrainComponent* Brain = GetBrainComponent())
	{
		Brain->SetComponentTickEnabled(true);
		Brain->RestartLogic();
	}

	bSuspendedForPooling = false;
}

void APeCoEnemyAIController::ResetBlackboardForCurrentPawn()
{
	UBlackboardComponent* BlackboardComp = GetBlackboardComponent();
	const APawn* ControlledPawn = GetPawn();
	if (!BlackboardComp || !ControlledPawn)
	{
		return;
	}

	// 새로 생성된 Controller가 받던 초기 상태와 풀 재사용 상태를 맞춘다.
	BlackboardComp->ClearValue(TEXT("PlayerLocation"));
	BlackboardComp->ClearValue(TEXT("LastKnownPlayerLocation"));
	BlackboardComp->SetValueAsVector(TEXT("StartLocation"), ControlledPawn->GetActorLocation());
}

void APeCoEnemyAIController::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
	if (PlayerPawn && GetBlackboardComponent())
	{
		if (LineOfSightTo(PlayerPawn))
		{
			GetBlackboardComponent()->SetValueAsVector(TEXT("PlayerLocation"), PlayerPawn->GetActorLocation());
			GetBlackboardComponent()->SetValueAsVector(TEXT("LastKnownPlayerLocation"), PlayerPawn->GetActorLocation());
		}
		else
		{
			GetBlackboardComponent()->ClearValue(TEXT("PlayerLocation"));
		}
	}

}
