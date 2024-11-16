// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "10_Enemy/PeCoEnemySpawner.h"
#include "PeCoGameMode.generated.h"

/**
 * 
 */
class APeCoEnemySpawner;

UCLASS()
class PESTCONTROL_API APeCoGameMode : public AGameMode
{
	GENERATED_BODY()

public:

	APeCoGameMode();

	void EnemyEliminated(AActor* EliminatedEnemy);

	//~AGameMode Interface

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	//~End of AGameMode Interface

	// Start Next Round
	void StartNextRound();


	float CalculateDamage(AController* Attacker, AController* Victim, float BaseDamage);

#pragma region Game Time & Timer

	// 1200.f = 1200초 = 20분
	UPROPERTY(EditDefaultsOnly)
	float TotalGameTime = 1200.f; 

	float LevelStartingTime = 0.f;

#pragma endregion


private:
	// 적 스폰을 위한 스폰 클래스
	UPROPERTY(EditAnywhere, Category = "Spawning")
	TSubclassOf<APeCoEnemySpawner> EnemySpawnerClass;

	// 현재 스폰된 적의 수를 추적
	int32 CurrentSpawnCount = 0;

	// 최대 스폰 가능한 적의 수
	int32 MaxSpawnCount = 10;

};
