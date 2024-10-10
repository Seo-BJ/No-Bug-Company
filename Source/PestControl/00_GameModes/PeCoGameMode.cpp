// Fill out your copyright notice in the Description page of Project Settings.


#include "00_GameModes/PeCoGameMode.h"

#include "01_Character/PeCoCharacter.h"
#include "01_Character/CombatInterface.h"

APeCoGameMode::APeCoGameMode()
{
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
	if (EnemySpawnerClass)
	{
		FVector SpawnLocation = FVector(2110.f, 1640.f, 100.f); 
		FRotator SpawnRotation = FRotator::ZeroRotator;

		APeCoEnemySpawner* Spawner = GetWorld()->SpawnActor<APeCoEnemySpawner>(EnemySpawnerClass, SpawnLocation, SpawnRotation);
		if (Spawner)
		{
			UE_LOG(LogTemp, Warning, TEXT("Enemy spawner created for new round"));
		}
	}
}

void APeCoGameMode::EnemyEliminated(AActor* EliminatedEnemy)
{
	// when enemy eliminated
	if (EliminatedEnemy)
	{
		CurrentSpawnCount--;

		UE_LOG(LogTemp, Warning, TEXT("Enemy eliminated. Remaining: %d"), CurrentSpawnCount);

		// 필요 시 스폰 로직을 추가하거나 다음 라운드를 시작
		if (CurrentSpawnCount <= 0)
		{
			StartNextRound();
		}
	}
}