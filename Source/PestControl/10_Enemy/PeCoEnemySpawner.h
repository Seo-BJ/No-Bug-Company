// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "01_Character/PeCoEnemyCharacter.h"
#include "10_Enemy/EnemyStats.h"
#include "PeCoEnemySpawner.generated.h"

UCLASS()
class PESTCONTROL_API APeCoEnemySpawner : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APeCoEnemySpawner();

    /*// Called every frame
    virtual void Tick(float DeltaTime) override;*/

    // 라운드 설정
    void SetRound(int32 NewRound);

    /*// 스폰할 적의 클래스 (BP로 설정 가능)
    UPROPERTY(EditAnywhere, Category = "Spawning")
    TSubclassOf<APeCoEnemyCharacter> EnemyClass;

    // 스폰 간격 (초 단위)
    UPROPERTY(EditAnywhere, Category = "Spawning")
    float SpawnInterval = 10.0f;

    // 한 번에 스폰할 적의 수
    UPROPERTY(EditAnywhere, Category = "Spawning")
    int32 SpawnCount = 3;*/
    // 스폰 간격
    UPROPERTY(EditAnywhere, Category = "Spawning")
    float SpawnInterval = 2.0f;

    // 한 번에 스폰할 적의 수
    UPROPERTY(EditAnywhere, Category = "Spawning")
    int32 SpawnCount = 1;

    

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:
    void SpawnEnemies();

    FTimerHandle SpawnTimerHandle;

    void UpdateEnemyPool();
    TSubclassOf<class APeCoEnemyCharacter> GetEnemyClassFromID(const FName& EnemyID);

    // 데이터 테이블 변수
    UPROPERTY(EditAnywhere, Category = "Data")
    UDataTable* EnemyStatsTable;

    // EnemyID와 클래스 매핑
    UPROPERTY(EditAnywhere, Category = "Data")
    TMap<FName, TSubclassOf<class APeCoEnemyCharacter>> EnemyIDToClassMap;

    // 현재 라운드 데이터
    TArray<FEnemyStats> CurrentRoundStats;

    // 현재 라운드
    int32 CurrentRound;

    
        
};
