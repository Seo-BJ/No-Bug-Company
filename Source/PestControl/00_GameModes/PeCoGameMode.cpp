// Fill out your copyright notice in the Description page of Project Settings.


#include "00_GameModes/PeCoGameMode.h"

#include "01_Character/PeCoCharacter.h"
#include "01_Character/CombatInterface.h"
#include "TimerManager.h"

APeCoGameMode::APeCoGameMode()
{
	CurrentRound = 0;
	RemainingEnemies = 0;
	bIsGameActive = true;
	CurrentRoundElapsedTime = 0.0f;

	// 라운드별 시간 설정
	RoundTimes = { 180.0f, 180.0f, 240.0f, 240.0f, 300.0f }; // 초 단위로 설정 (3분, 3분, 4분, 4분, 5분)

}

void APeCoGameMode::BeginPlay()
{
	Super::BeginPlay();
	LevelStartingTime = GetWorld()->GetTimeSeconds();
	

	// 스포너 생성
	FVector SpawnLocation(2110.f, 1640.f, 100.f);
	FRotator SpawnRotation = FRotator::ZeroRotator;
	
	if (EnemySpawnerClass)
	{
		EnemySpawnerInstance = GetWorld()->SpawnActor<APeCoEnemySpawner>(EnemySpawnerClass, SpawnLocation, SpawnRotation);

		if (EnemySpawnerInstance)
		{
			UE_LOG(LogTemp, Log, TEXT("EnemySpawnerInstance successfully created."));
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("Failed to create EnemySpawnerInstance."));
		}
	}

	else
	{
		UE_LOG(LogTemp, Error, TEXT("EnemySpawnerClass is not set in GameMode!"));
	}
	
	StartNextRound();


}

void APeCoGameMode::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// 게임 시간이 다 되었는지 확인
	CheckGameOver();

	// 라운드 시간이 초과되었는지 확인
	CheckRoundTimer(DeltaTime);
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

void APeCoGameMode::StartNextRound()
{
	/*if (EnemySpawnerClass)
	{
		FVector SpawnLocation = FVector(2110.f, 1640.f, 100.f); 
		FRotator SpawnRotation = FRotator::ZeroRotator;

		APeCoEnemySpawner* Spawner = GetWorld()->SpawnActor<APeCoEnemySpawner>(EnemySpawnerClass, SpawnLocation, SpawnRotation);
		if (Spawner)
		{
			UE_LOG(LogTemp, Warning, TEXT("Enemy spawner created for new round"));
		}
	}*/


	// 라운드 증가
	CurrentRound++;

	// 라운드가 최대 라운드를 초과했는지 확인
	if (CurrentRound > MaxRounds)
	{
		UE_LOG(LogTemp, Log, TEXT("Game Over: All rounds completed!"));
		bIsGameActive = false;
		return;
	}

	// 라운드 초기화
	UE_LOG(LogTemp, Log, TEXT("Starting Round %d"), CurrentRound);

	// 라운드 경과 시간 초기화
	CurrentRoundElapsedTime = 0.0f;

	// 스포너에서 라운드 설정
	if (EnemySpawnerInstance)
	{
		EnemySpawnerInstance->SetRound(CurrentRound);

		// 라운드의 적 수 설정
		RemainingEnemies = EnemySpawnerInstance->SpawnCount;
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("No Enemy Spawner found in GameMode!"));
	}
}

void APeCoGameMode::EnemyEliminated(AActor* EliminatedEnemy)
{
	if (!bIsGameActive) return;

	// 남은 적 수 감소
	RemainingEnemies--;

	UE_LOG(LogTemp, Log, TEXT("Enemy Eliminated. Remaining: %d"), RemainingEnemies);

	// 모든 적이 제거되었는지 확인
	if (RemainingEnemies <= 0)
	{
		UE_LOG(LogTemp, Log, TEXT("All enemies eliminated for round %d"), CurrentRound);

		// 라운드 간 대기 후 다음 라운드 시작
		GetWorld()->GetTimerManager().SetTimer(RoundTimerHandle, this, &APeCoGameMode::StartNextRound, TimeBetweenRounds, false);
	}
	
	
	/*// when enemy eliminated
	if (EliminatedEnemy)
	{
		CurrentSpawnCount--;

		UE_LOG(LogTemp, Warning, TEXT("Enemy eliminated. Remaining: %d"), CurrentSpawnCount);

		// 필요 시 스폰 로직을 추가하거나 다음 라운드를 시작
		if (CurrentSpawnCount <= 0)
		{
			StartNextRound();
		}
	}*/
}

void APeCoGameMode::CheckGameOver()
{
	float ElapsedTime = GetWorld()->GetTimeSeconds() - LevelStartingTime;
	if (ElapsedTime >= TotalGameTime)
	{
		UE_LOG(LogTemp, Log, TEXT("Game Over: Time expired!"));
		bIsGameActive = false;
	}
}

void APeCoGameMode::CheckRoundTimer(float DeltaTime)
{
	if (!bIsGameActive || CurrentRound > MaxRounds) return;

	// 현재 라운드 시간 업데이트
	CurrentRoundElapsedTime += DeltaTime;

	// 현재 라운드의 최대 시간을 초과했는지 확인
	if (CurrentRoundElapsedTime >= RoundTimes[CurrentRound - 1])
	{
		UE_LOG(LogTemp, Log, TEXT("Time's up for round %d"), CurrentRound);
		StartNextRound();
	}
}