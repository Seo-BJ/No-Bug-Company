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

    // 벤치마크: 스폰된 적에게 자동 사망 수명을 주입. 0이면 미주입(=CVar 값 사용).
    UPROPERTY(EditAnywhere, Category = "Benchmark")
    float SpawnedAutoKillLifetime = 0.f;



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

    
        
};
