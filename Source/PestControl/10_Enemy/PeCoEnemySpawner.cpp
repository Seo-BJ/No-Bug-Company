// Fill out your copyright notice in the Description page of Project Settings.

#include "10_Enemy/PeCoEnemySpawner.h"

#include "PestControl.h"
#include "00_GameModes/PeCoGameMode.h"

#include "10_Enemy/PeCoGroundEnemyCharacter.h"
#include "10_Enemy/PeCoFlyingEnemyCharacter.h"

#include "10_Enemy/PeCoSpiderCharacter.h"
#include "10_Enemy/PeCoMosquitoCharacter.h"
#include "10_Enemy/BossEnemy.h"

#include "20_System/Pool/PeCoPoolSubsystem.h"

#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"
#include "TimerManager.h"

APeCoEnemySpawner::APeCoEnemySpawner()
{
	PrimaryActorTick.bCanEverTick = true;

    // EnemyID와 클래스 매핑 초기화
    EnemyIDToClassMap.Add("Spider", APeCoSpiderCharacter::StaticClass());
    EnemyIDToClassMap.Add("Mosquito", APeCoMosquitoCharacter::StaticClass());
    EnemyIDToClassMap.Add("Boss", ABossEnemy::StaticClass()); 
}

void APeCoEnemySpawner::BeginPlay()
{
	Super::BeginPlay();

    // 초기 라운드를 설정
    SetRound(); // 기본 라운드를 1로 설정

    // 고정 주기 루프 타이머.
    GetWorld()->GetTimerManager().SetTimer(SpawnTimerHandle, this, &APeCoEnemySpawner::SpawnEnemies, SpawnInterval, true);

}

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

    // 클래스별 PreWarm/확장/보관 정책은 현재 레벨의 Pool Profile에서 관리한다.
}

bool APeCoEnemySpawner::IsPoolableEnemyClass(TSubclassOf<APeCoEnemyCharacter> Class)
{
    if (!Class)
    {
        return false;
    }
    // 보스는 고유 상태가 많고 재사용 이득이 적어 풀링 제외.
    if (Class->IsChildOf(ABossEnemy::StaticClass()))
    {
        return false;
    }
    return true;
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

        if (TSubclassOf<APeCoEnemyCharacter> EnemyClass = GetEnemyClassFromID(Stats.EnemyID))
        {
            // 기본 스폰 위치와 회전
            FVector SpawnLocation = GetActorLocation();
            FRotator SpawnRotation = GetActorRotation();

            // 약간의 랜덤 오프셋 추가
            FVector Offset = FVector(FMath::RandRange(-100, 100), FMath::RandRange(-100, 100), 0);
            FVector FinalSpawnLocation = SpawnLocation + Offset;

            // 적 생성: 풀링 대상이면 PoolSubsystem->Acquire, 아니면 기존 SpawnActor.
            APeCoEnemyCharacter* SpawnedEnemy = nullptr;
            const FTransform SpawnT(SpawnRotation, FinalSpawnLocation);
            
            if (IsPoolableEnemyClass(EnemyClass))
            {
                if (UPeCoPoolSubsystem* Pool = GetWorld()->GetSubsystem<UPeCoPoolSubsystem>())
                {
                    SpawnedEnemy = Pool->Acquire<APeCoEnemyCharacter>(EnemyClass, SpawnT, this, nullptr);
                }
            }
            else
            {
                // 비 풀링 대상(보스 등)은 기존 SpawnActor 경로 사용.
                SpawnedEnemy = GetWorld()->SpawnActor<APeCoEnemyCharacter>(EnemyClass, FinalSpawnLocation, SpawnRotation);
            }

            if (SpawnedEnemy)
            {
                SpawnedEnemy->ApplyStatsFromData(Stats);
            }
        }
    }
}
