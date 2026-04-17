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

    // ���� ����
    void SetRound();

    /*// ������ ���� Ŭ���� (BP�� ���� ����)
    UPROPERTY(EditAnywhere, Category = "Spawning")
    TSubclassOf<APeCoEnemyCharacter> EnemyClass;

    // ���� ���� (�� ����)
    UPROPERTY(EditAnywhere, Category = "Spawning")
    float SpawnInterval = 10.0f;

    // �� ���� ������ ���� ��
    UPROPERTY(EditAnywhere, Category = "Spawning")
    int32 SpawnCount = 3;*/
    // ���� ����
    UPROPERTY(EditAnywhere, Category = "Spawning")
    float SpawnInterval = 2.0f;

    // �� ���� ������ ���� ��
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

    // ������ ���̺� ����
    UPROPERTY(EditAnywhere, Category = "Data")
    UDataTable* EnemyStatsTable;

    // EnemyID�� Ŭ���� ����
    UPROPERTY(EditAnywhere, Category = "Data")
    TMap<FName, TSubclassOf<class APeCoEnemyCharacter>> EnemyIDToClassMap;

    // ���� ���� ������
    TArray<FEnemyStats> CurrentRoundStats;

    // ���� ����

    int32 CurrentRound;

    // 라운드 시작 시 Enemy 클래스별로 미리 생성해둘 개수.
    // 0이면 PreWarm을 건너뛴다. 첫 웨이브의 SpawnActor 스파이크를 줄이는 용도.
    UPROPERTY(EditAnywhere, Category = "Spawning|Pool")
    int32 PreWarmCountPerClass = 10;

    // 클래스별 풀 보관 상한. 0 = 무제한.
    UPROPERTY(EditAnywhere, Category = "Spawning|Pool")
    int32 PoolMaxSizePerClass = 0;

    // 보스는 풀링 대상에서 제외 (인스턴스당 상태가 크고 재사용 이득이 적음).
    static bool IsPoolableEnemyClass(TSubclassOf<class APeCoEnemyCharacter> Class);
};
