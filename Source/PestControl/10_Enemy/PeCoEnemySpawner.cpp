// Fill out your copyright notice in the Description page of Project Settings.


#include "10_Enemy/PeCoEnemySpawner.h"
#include "01_Character/PeCoEnemyCharacter.h"

#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"
#include "TimerManager.h"


// Sets default values
APeCoEnemySpawner::APeCoEnemySpawner()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void APeCoEnemySpawner::BeginPlay()
{
	Super::BeginPlay();

	// Set a timer to spawn enemies at regular intervals
	GetWorld()->GetTimerManager().SetTimer(SpawnTimerHandle, this, &APeCoEnemySpawner::SpawnEnemies, SpawnInterval, true);
	
}

// Called every frame
void APeCoEnemySpawner::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void APeCoEnemySpawner::SpawnEnemies()
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
}

