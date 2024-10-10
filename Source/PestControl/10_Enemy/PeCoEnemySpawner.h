// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "01_Character/PeCoEnemyCharacter.h"
#include "PeCoEnemySpawner.generated.h"

UCLASS()
class PESTCONTROL_API APeCoEnemySpawner : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APeCoEnemySpawner();

    // Called every frame
    virtual void Tick(float DeltaTime) override;

    // 스폰할 적의 클래스 (BP로 설정 가능)
    UPROPERTY(EditAnywhere, Category = "Spawning")
    TSubclassOf<APeCoEnemyCharacter> EnemyClass;

    // 스폰 간격 (초 단위)
    UPROPERTY(EditAnywhere, Category = "Spawning")
    float SpawnInterval = 10.0f;

    // 한 번에 스폰할 적의 수
    UPROPERTY(EditAnywhere, Category = "Spawning")
    int32 SpawnCount = 3;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:
    void SpawnEnemies();

    FTimerHandle SpawnTimerHandle;

};
