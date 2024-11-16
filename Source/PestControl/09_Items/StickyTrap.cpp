// Fill out your copyright notice in the Description page of Project Settings.


#include "09_Items/StickyTrap.h"

#include "01_Character/PeCoEnemyCharacter.h"

#include "TimerManager.h"

#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"

AStickyTrap::AStickyTrap()
{
    PrimaryActorTick.bCanEverTick = false;

    // Root component
    RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));

    // Trap Mesh
    TrapMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("TrapMesh"));
    TrapMesh->SetupAttachment(RootComponent);

    // Detection Box
    DetectionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("DetectionBox"));
    DetectionBox->SetupAttachment(TrapMesh);
    DetectionBox->SetBoxExtent(FVector(200.f, 200.f, 10.f));

    DetectionBox->OnComponentBeginOverlap.AddDynamic(this, &AStickyTrap::OnEnemyOverlapped);

    // Initial state
    bIsTrapActive = false;
}

void AStickyTrap::BeginPlay()
{
    Super::BeginPlay();
}

void AStickyTrap::OnEnemyOverlapped(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    if (OtherActor)
    {
        APeCoEnemyCharacter* EnemyCharacter = Cast<APeCoEnemyCharacter>(OtherActor);
        if (!EnemyCharacter)
        {
            UE_LOG(LogTemp, Error, TEXT("OtherActor is not APeCoEnemyCharacter!"));
            return;
        }

        UCharacterMovementComponent* MovementComponent = EnemyCharacter->GetCharacterMovement();
        if (!MovementComponent)
        {
            UE_LOG(LogTemp, Error, TEXT("EnemyCharacter has no valid MovementComponent!"));
            return;
        }

        if (!bIsTrapActive && OverlappingEnemies.Num() < 10)
        {
            OverlappingEnemies.Add(EnemyCharacter);
            OriginalSpeeds.Add(EnemyCharacter, MovementComponent->MaxWalkSpeed);
            DisableMove();
            ActivateTrap();
        }
        else if (OverlappingEnemies.Num() < 10)
        {
            OverlappingEnemies.Add(EnemyCharacter);
            OriginalSpeeds.Add(EnemyCharacter, MovementComponent->MaxWalkSpeed);
            DisableMove();
        }
    }
}

void AStickyTrap::ActivateTrap()
{
    if (!bIsTrapActive)
    {
        bIsTrapActive = true;

        FTimerHandle DeactivateTimerHandle;

        GetWorld()->GetTimerManager().SetTimer(DeactivateTimerHandle, this, &AStickyTrap::DeactivateTrap, TrapDuration, false);
    }
}

void AStickyTrap::DeactivateTrap()
{
    EnableMove();

    Destroy();
}

void AStickyTrap::DisableMove()
{
    for (APeCoEnemyCharacter* Enemy : OverlappingEnemies)
    {
        UCharacterMovementComponent* MovementComponent = Enemy->GetCharacterMovement();
        if (MovementComponent)
        {
            bool bIsFlyingEnemy = MovementComponent->MovementMode == EMovementMode::MOVE_Flying;

            if (bIsFlyingEnemy)
            {
                MovementComponent->StopMovementImmediately();
                MovementComponent->MaxFlySpeed = 0.0f;
            }
            else
            {
                MovementComponent->StopMovementImmediately();
                MovementComponent->MaxWalkSpeed = 0.0f;
            }
        }
    }
}

void AStickyTrap::EnableMove()
{
    for (APeCoEnemyCharacter* Enemy : OverlappingEnemies)
    {
        UCharacterMovementComponent* MovementComponent = Enemy->GetCharacterMovement();
        if (MovementComponent)
        {
            bool bIsFlyingEnemy = MovementComponent->MovementMode == EMovementMode::MOVE_Flying;

            if (bIsFlyingEnemy)
            {
                MovementComponent->MaxFlySpeed = OriginalSpeeds[Enemy];
                UE_LOG(LogTemp, Warning, TEXT("Restored Fly Speed for %s to %f"), *Enemy->GetName(), OriginalSpeeds[Enemy]);
            }
            else
            {
                MovementComponent->MaxWalkSpeed = OriginalSpeeds[Enemy];
                UE_LOG(LogTemp, Warning, TEXT("Restored Walk Speed for %s to %f"), *Enemy->GetName(), OriginalSpeeds[Enemy]);
            }
        }
    }
    OverlappingEnemies.Empty();
    OriginalSpeeds.Empty();
}

