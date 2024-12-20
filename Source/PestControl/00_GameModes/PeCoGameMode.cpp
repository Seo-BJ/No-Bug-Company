// Fill out your copyright notice in the Description page of Project Settings.


#include "PeCoGameMode.h"
#include "00_GameModes/Components/StoreComponent.h"
#include "01_Character/PeCoCharacter.h"
#include "01_Character/CombatInterface.h"
#include "TimerManager.h"
#include "Kismet/GameplayStatics.h"

#include "21_Data/PeCoDataTypes.h"

APeCoGameMode::APeCoGameMode()
{
	CurrentRound = 0;
	RemainingEnemies = 0;
	bIsGameActive = true;
	CurrentRoundElapsedTime = 0.0f;

	// 라운드별 시간 설정
	RoundTimes = { 180.0f, 180.0f, 240.0f, 240.0f, 300.0f }; // 초 단위로 설정 (3분, 3분, 4분, 4분, 5분)
	StoreComponent = CreateDefaultSubobject<UStoreComponent>(TEXT("StoreComponent"));
}

void APeCoGameMode::BeginPlay()
{
	Super::BeginPlay();
	LevelStartingTime = GetWorld()->GetTimeSeconds();
	
	
	StartNextRound();


}

void APeCoGameMode::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// 게임 시간이 다 되었는지 확인
	if (bTimeOver == false || bStopTimer == false)
	{
		CheckGameOver();
	}

}

float APeCoGameMode::CalculateDamage(AController* Attacker, AController* Victim, float BaseDamage)
{
	if (Attacker == nullptr || Victim == nullptr)
	{
		return BaseDamage;
	}
	APawn* AttackerPawn = Attacker->GetPawn();
	APawn* VictimPawn = Victim->GetPawn();
	if (AttackerPawn && VictimPawn)
	{
		APeCoCharacter* AttackerCharacter = Cast<APeCoCharacter>(AttackerPawn);
		APeCoCharacter* VictimCharacter = Cast<APeCoCharacter>(VictimPawn);
		
		if (AttackerCharacter == VictimCharacter)
		{
			return BaseDamage;
		}
		if (AttackerCharacter->GetTeam() == VictimCharacter->GetTeam())
		{
			UE_LOG(LogTemp, Warning, TEXT("Same team, setting damage to 0"));
			return 0.f;
		}
	}
	return BaseDamage;
}


void APeCoGameMode::StageTimeEnd()
{
	OnStageTimeEnd.Broadcast(StageNumber);
	

}

void APeCoGameMode::StageFinishAndStartNextStage_Implementation()
{
	bStopTimer = true;

	CleanupEnemies(); // 적 및 AI 정리

}



void APeCoGameMode::StartNextRound()
{
	// 라운드 경과 시간 초기화
	CurrentRoundElapsedTime = 0.0f;
}

void APeCoGameMode::EnemyEliminated(AActor* EliminatedEnemy)
{
	if (!bIsGameActive) return;

	// 남은 적 수 감소
	RemainingEnemies--;

}

void APeCoGameMode::CheckGameOver()
{
	float ElapsedTime = GetWorld()->GetTimeSeconds() - LevelStartingTime;
	if (ElapsedTime >= StageTimeLimit)
	{
		StageTimeEnd();
		bTimeOver = true;

	}
}

void APeCoGameMode::CleanupEnemies()
{
	// 모든 적과 AIController 정리
	TArray<AActor*> EnemyActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), APeCoEnemyCharacter::StaticClass(), EnemyActors);

	for (AActor* EnemyActor : EnemyActors)
	{
		APeCoEnemyCharacter* EnemyCharacter = Cast<APeCoEnemyCharacter>(EnemyActor);
		if (EnemyCharacter)
		{
			// 타이머 정리
			GetWorld()->GetTimerManager().ClearAllTimersForObject(EnemyCharacter);

			// 적 제거
			EnemyCharacter->Destroy();
		}
	}

	UE_LOG(LogTemp, Log, TEXT("All enemies have been cleaned up."));
}