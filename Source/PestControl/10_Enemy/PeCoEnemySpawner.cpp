// Fill out your copyright notice in the Description page of Project Settings.


#include "10_Enemy/PeCoEnemySpawner.h"
#include "10_Enemy/PeCoGroundEnemyCharacter.h"
#include "10_Enemy/PeCoFlyingEnemyCharacter.h"

#include "10_Enemy/PeCoLarvaCharacter.h"
#include "10_Enemy/PeCoFlyCharacter.h"
#include "10_Enemy/PeCoSpiderCharacter.h"
#include "10_Enemy/PeCoRoachCharacter.h"
#include "10_Enemy/PeCoMosquitoCharacter.h"

#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"
#include "TimerManager.h"


// Sets default values
APeCoEnemySpawner::APeCoEnemySpawner()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

    // EnemyID와 클래스 매핑 초기화
    EnemyIDToClassMap.Add("Larva", APeCoLarvaCharacter::StaticClass());
    EnemyIDToClassMap.Add("Fly", APeCoFlyCharacter::StaticClass());
    EnemyIDToClassMap.Add("Spider", APeCoSpiderCharacter::StaticClass());
    EnemyIDToClassMap.Add("Roach", APeCoRoachCharacter::StaticClass());
    EnemyIDToClassMap.Add("Mosquito", APeCoMosquitoCharacter::StaticClass());


}

// Called when the game starts or when spawned
void APeCoEnemySpawner::BeginPlay()
{
	Super::BeginPlay();

	// Set a timer to spawn enemies at regular intervals
	GetWorld()->GetTimerManager().SetTimer(SpawnTimerHandle, this, &APeCoEnemySpawner::SpawnEnemies, SpawnInterval, true);
	
}

/*// Called every frame
void APeCoEnemySpawner::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}*/

void APeCoEnemySpawner::SetRound(int32 NewRound)
{
    CurrentRound = NewRound;
    UpdateEnemyPool();
}

void APeCoEnemySpawner::UpdateEnemyPool()
{
    if (!EnemyStatsTable) return;

    CurrentRoundStats.Empty();

    static const FString ContextString(TEXT("Enemy Stats Context"));
    TArray<FEnemyStats*> AllRows;
    EnemyStatsTable->GetAllRows<FEnemyStats>(ContextString, AllRows);

    for (FEnemyStats* Stats : AllRows)
    {
        if (Stats && Stats->Round == CurrentRound)
        {
            CurrentRoundStats.Add(*Stats);
        }
    }

    UE_LOG(LogTemp, Log, TEXT("Loaded %d enemy stats for round %d"), CurrentRoundStats.Num(), CurrentRound);
}

TSubclassOf<APeCoEnemyCharacter> APeCoEnemySpawner::GetEnemyClassFromID(const FName& EnemyID)
{
    if (EnemyIDToClassMap.Contains(EnemyID))
    {
        return EnemyIDToClassMap[EnemyID];
    }

    UE_LOG(LogTemp, Error, TEXT("EnemyID %s not found in map!"), *EnemyID.ToString());
    return nullptr;
}



void APeCoEnemySpawner::SpawnEnemies()
{
    if (CurrentRoundStats.Num() == 0)
    {
        UE_LOG(LogTemp, Warning, TEXT("No enemies to spawn for round %d"), CurrentRound);
        return;
    }

    for (int32 i = 0; i < SpawnCount; ++i)
    {
        // 라운드 적 데이터에서 랜덤 선택
        const FEnemyStats& Stats = CurrentRoundStats[FMath::RandRange(0, CurrentRoundStats.Num() - 1)];
        TSubclassOf<APeCoEnemyCharacter> EnemyClass = GetEnemyClassFromID(Stats.EnemyID);

        if (EnemyClass)
        {
            // 기본 스폰 위치와 회전
            FVector SpawnLocation = GetActorLocation();
            FRotator SpawnRotation = GetActorRotation();

            // 약간의 랜덤 오프셋 추가
            FVector Offset = FVector(FMath::RandRange(-100, 100), FMath::RandRange(-100, 100), 0);
            FVector FinalSpawnLocation = SpawnLocation + Offset;

            // 적 생성
            APeCoEnemyCharacter* SpawnedEnemy = GetWorld()->SpawnActor<APeCoEnemyCharacter>(EnemyClass, FinalSpawnLocation, SpawnRotation);

            if (SpawnedEnemy)
            {
                SpawnedEnemy->ApplyStatsFromData(Stats);
                UE_LOG(LogTemp, Log, TEXT("Spawned enemy: %s with health %.2f"), *SpawnedEnemy->GetName(), Stats.Health);
                //UE_LOG(LogTemp, Log, TEXT("Spawned enemy: %s"), *SpawnedEnemy->GetName());
            }
            else
            {
                UE_LOG(LogTemp, Error, TEXT("Failed to spawn enemy at location %s"), *FinalSpawnLocation.ToString());
            }
        }
    }
}

void APeCoEnemySpawner::UpdateStatsForHealthIncrease(float Percentage, FName TargetEnemyID)
{
    for (FEnemyStats& Stats : CurrentRoundStats)
    {
        if (Stats.EnemyID == TargetEnemyID)
        {
            Stats.Health += Stats.Health * (Percentage / 100.0f);
            UE_LOG(LogTemp, Log, TEXT("Updated stats: EnemyID %s Health increased to %.2f"), *TargetEnemyID.ToString(), Stats.Health);
        }
    }
}