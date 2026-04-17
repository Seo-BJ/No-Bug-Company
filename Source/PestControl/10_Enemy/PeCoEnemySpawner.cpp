// Fill out your copyright notice in the Description page of Project Settings.



#include "10_Enemy/PeCoEnemySpawner.h"

#include "00_GameModes/PeCoGameMode.h"

#include "10_Enemy/PeCoGroundEnemyCharacter.h"
#include "10_Enemy/PeCoFlyingEnemyCharacter.h"

#include "10_Enemy/PeCoSpiderCharacter.h"
#include "10_Enemy/PeCoMosquitoCharacter.h"
#include "10_Enemy/BossEnemy.h"

#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"
#include "TimerManager.h"
#include "HAL/PlatformTime.h"
#include "20_System/PeCoBenchmarkSubsystem.h"


// Sets default values
APeCoEnemySpawner::APeCoEnemySpawner()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

    // EnemyID와 클래스 매핑 초기화
    EnemyIDToClassMap.Add("Spider", APeCoSpiderCharacter::StaticClass());
    EnemyIDToClassMap.Add("Mosquito", APeCoMosquitoCharacter::StaticClass());
    EnemyIDToClassMap.Add("Boss", ABossEnemy::StaticClass()); 

}

// Called when the game starts or when spawned
void APeCoEnemySpawner::BeginPlay()
{
	Super::BeginPlay();

    // 초기 라운드를 설정
    SetRound(); // 기본 라운드를 1로 설정

	// Set a timer to spawn enemies at regular intervals
	GetWorld()->GetTimerManager().SetTimer(SpawnTimerHandle, this, &APeCoEnemySpawner::SpawnEnemies, SpawnInterval, true);
	
}

/*// Called every frame
void APeCoEnemySpawner::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}*/

void APeCoEnemySpawner::SetRound()
{
    APeCoGameMode* GameMode = Cast<APeCoGameMode>(UGameplayStatics::GetGameMode(this));
    if (GameMode)
    {
        CurrentRound = GameMode->StageNumber;
    }
    UE_LOG(LogTemp, Warning, TEXT("Current Round : %d"), CurrentRound);
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
        if (Stats && Stats->Round == CurrentRound && !Stats->bIsBoss)
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

/*void APeCoEnemySpawner::SpawnEnemies()
{
    if (!GetWorld())
    {
        UE_LOG(LogTemp, Error, TEXT("GetWorld() returned nullptr in %s"), *GetName());
        return;
    }
    
    if (EnemyClass == nullptr)
    {
        UE_LOG(LogTemp, Error, TEXT("EnemyClass is not set in %s"), *GetName());
        return;
    }
        
    // Calculate the spawn location based on the spawner's location
    FVector SpawnLocation = GetActorLocation();
    FRotator SpawnRotation = GetActorRotation();

    for (int32 i = 0; i < SpawnCount; ++i)
    {
        // Add a slight offset to prevent enemies from spawning on top of each other
        FVector Offset = FVector(FMath::RandRange(-100, 100), FMath::RandRange(-100, 100), 0);
        FVector FinalSpawnLocation = SpawnLocation + Offset;
                
        APeCoEnemyCharacter* SpawnedEnemy = GetWorld()->SpawnActor<APeCoEnemyCharacter>(EnemyClass, FinalSpawnLocation, SpawnRotation); 
        if (SpawnedEnemy)
        {
            UE_LOG(LogTemp, Warning, TEXT("Spawned an enemy: %s"), *SpawnedEnemy->GetName());
        }
        else
        {
            UE_LOG(LogTemp, Error, TEXT("Failed to spawn enemy at location: %s"), *FinalSpawnLocation.ToString());
        }
    }
}*/

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

            // Deferred 스폰으로 BeginPlay 전에 AutoKillLifetime을 주입할 수 있게 한다.
            const double SpawnStartSeconds = FPlatformTime::Seconds();
            const FTransform SpawnTransform(SpawnRotation, FinalSpawnLocation);
            APeCoEnemyCharacter* SpawnedEnemy = GetWorld()->SpawnActorDeferred<APeCoEnemyCharacter>(
                EnemyClass,
                SpawnTransform,
                nullptr,
                nullptr,
                ESpawnActorCollisionHandlingMethod::AlwaysSpawn);

            if (SpawnedEnemy)
            {
                if (SpawnedAutoKillLifetime > 0.f)
                {
                    SpawnedEnemy->AutoKillLifetime = SpawnedAutoKillLifetime;
                }
                SpawnedEnemy->FinishSpawning(SpawnTransform);
                SpawnedEnemy->ApplyStatsFromData(Stats);
                UE_LOG(LogTemp, Log, TEXT("Spawned enemy: %s with health %.2f"), *SpawnedEnemy->GetName(), Stats.Health);
            }

            const double SpawnElapsedUs = (FPlatformTime::Seconds() - SpawnStartSeconds) * 1'000'000.0;
            if (UPeCoBenchmarkSubsystem* Bench = UPeCoBenchmarkSubsystem::Get(this))
            {
                Bench->RecordEnemySpawn(SpawnElapsedUs);
            }
            else
            {
                UE_LOG(LogTemp, Error, TEXT("Failed to spawn enemy at location %s"), *FinalSpawnLocation.ToString());
            }
        }
    }
}