#include "09_Items/StickyTrap.h"

#include "01_Character/PeCoEnemyCharacter.h"

#include "TimerManager.h"

#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"

AStickyTrap::AStickyTrap()
{
    PrimaryActorTick.bCanEverTick = false;

    RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));

    TrapMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("TrapMesh"));
    TrapMesh->SetupAttachment(RootComponent);

    DetectionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("DetectionBox"));
    DetectionBox->SetupAttachment(TrapMesh);
    DetectionBox->SetBoxExtent(FVector(200.f, 200.f, 10.f));

    DetectionBox->OnComponentBeginOverlap.AddDynamic(this, &AStickyTrap::OnEnemyOverlapped);

    bIsTrapActive = false;
}

void AStickyTrap::BeginPlay()
{
    Super::BeginPlay();
}

void AStickyTrap::OnEnemyOverlapped(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    if (!OtherActor || !OverlappedComp)
    {
        UE_LOG(LogTemp, Warning, TEXT("Invalid overlap detected: Null Actor or Component"));
        return;
    }

    APeCoEnemyCharacter* EnemyCharacter = Cast<APeCoEnemyCharacter>(OtherActor);
    if (!EnemyCharacter)
    {
        UE_LOG(LogTemp, Warning, TEXT("Overlapping actor is not a valid enemy character"));
        return;
    }

    UCharacterMovementComponent* MovementComponent = EnemyCharacter->GetCharacterMovement();
    if (!MovementComponent)
    {
        UE_LOG(LogTemp, Warning, TEXT("Enemy character does not have a valid movement component"));
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
        if (!Enemy)
        {
            UE_LOG(LogTemp, Warning, TEXT("Null enemy in OverlappingEnemies list"));
            continue;
        }

        UCharacterMovementComponent* MovementComponent = Enemy->GetCharacterMovement();
        if (!MovementComponent)
        {
            UE_LOG(LogTemp, Warning, TEXT("Enemy character does not have a valid movement component"));
            continue;
        }

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

void AStickyTrap::EnableMove()
{
    for (APeCoEnemyCharacter* Enemy : OverlappingEnemies)
    {
        if (!Enemy)
        {
            UE_LOG(LogTemp, Warning, TEXT("Null enemy in OverlappingEnemies list"));
            continue;
        }

        UCharacterMovementComponent* MovementComponent = Enemy->GetCharacterMovement();
        if (!MovementComponent)
        {
            UE_LOG(LogTemp, Warning, TEXT("Enemy character does not have a valid movement component"));
            continue;
        }

        bool bIsFlyingEnemy = MovementComponent->MovementMode == EMovementMode::MOVE_Flying;

        if (bIsFlyingEnemy)
        {
            if (OriginalSpeeds.Contains(Enemy))
            {
                MovementComponent->MaxFlySpeed = OriginalSpeeds[Enemy];
                UE_LOG(LogTemp, Warning, TEXT("Restored Fly Speed for %s to %f"), *Enemy->GetName(), OriginalSpeeds[Enemy]);
            }
            else
            {
                UE_LOG(LogTemp, Warning, TEXT("Original fly speed not found for %s"), *Enemy->GetName());
            }
        }
        else
        {
            if (OriginalSpeeds.Contains(Enemy))
            {
                MovementComponent->MaxWalkSpeed = OriginalSpeeds[Enemy];
                UE_LOG(LogTemp, Warning, TEXT("Restored Walk Speed for %s to %f"), *Enemy->GetName(), OriginalSpeeds[Enemy]);
            }
            else
            {
                UE_LOG(LogTemp, Warning, TEXT("Original walk speed not found for %s"), *Enemy->GetName());
            }
        }
    }
    OverlappingEnemies.Empty();
    OriginalSpeeds.Empty();
}