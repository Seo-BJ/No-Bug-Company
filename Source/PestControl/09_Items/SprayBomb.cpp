// Fill out your copyright notice in the Description page of Project Settings.


#include "09_Items/SprayBomb.h"

#include "01_Character/PeCoEnemyCharacter.h"
#include "01_Character/PeCoPlayerCharacter.h"

#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"

#include "Kismet/GameplayStatics.h"

// Sets default values
ASprayBomb::ASprayBomb()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

    BombMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BombMesh"));
    RootComponent = BombMesh;

    BombMesh->SetSimulatePhysics(true);
    BombMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);

    ExplodeRange = CreateDefaultSubobject<USphereComponent>(TEXT("DetectionSphere"));
    ExplodeRange->SetupAttachment(BombMesh);
    
}

// Called when the game starts or when spawned
void ASprayBomb::BeginPlay()
{
	Super::BeginPlay();

    if (IsValid(this) && IsValid(BombMesh))
    {
        BombMesh->OnComponentHit.AddDynamic(this, &ASprayBomb::OnHit);
    }
}

// Called every frame
void ASprayBomb::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ASprayBomb::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
    UE_LOG(LogTemp, Warning, TEXT("On Hit Called"));

    if (OtherActor && OtherActor != this && OtherComp)
    {
        UE_LOG(LogTemp, Log, TEXT("BombMesh hit: %s"), *OtherActor->GetName());

        // 충돌 시 폭발 함수 호출
        Explode();
    }
}


void ASprayBomb::Explode()
{
    TArray<AActor*> ActorsInRange;
    ExplodeRange->GetOverlappingActors(ActorsInRange);

    for (AActor* Actor : ActorsInRange)
    {
        APeCoEnemyCharacter* EnemyCharacter = Cast<APeCoEnemyCharacter>(Actor);

        if (EnemyCharacter)
        {
            float Damage = EnemyCharacter->GetMaxHealth() * MaxHealthDamageMultiplier;

            UGameplayStatics::ApplyDamage(
                Actor,
                Damage,
                nullptr,
                this,
                UDamageType::StaticClass()
            );

            ApplySlowEffect(EnemyCharacter);
        }
    }

    BombMesh->OnComponentHit.RemoveDynamic(this, &ASprayBomb::OnHit);
    Destroy();
}

void ASprayBomb::ApplySlowEffect(APeCoEnemyCharacter* EnemyCharacter)
{
    {
        if (bApplySlowEffect && EnemyCharacter && !EnemyCharacter->bIsSlowed)
        {
            UCharacterMovementComponent* MovementComponent = EnemyCharacter->GetCharacterMovement();
            if (MovementComponent)
            {
                bool bIsFlyingEnemy = MovementComponent->MovementMode == EMovementMode::MOVE_Flying;

                float OriginalSpeed = bIsFlyingEnemy ? MovementComponent->MaxFlySpeed : MovementComponent->MaxWalkSpeed;

                if (bIsFlyingEnemy)
                {
                    MovementComponent->MaxFlySpeed *= SlowMultiplier;
                }
                else
                {
                    MovementComponent->MaxWalkSpeed *= SlowMultiplier;
                }

                EnemyCharacter->bIsSlowed = true;

                FTimerHandle ResetSpeedHandle;
                GetWorld()->GetTimerManager().SetTimer(ResetSpeedHandle, FTimerDelegate::CreateLambda([=]() {

                    if (bIsFlyingEnemy)
                    {
                        MovementComponent->MaxFlySpeed = OriginalSpeed;
                    }
                    else
                    {
                        MovementComponent->MaxWalkSpeed = OriginalSpeed;
                    }
                    EnemyCharacter->ResetSlowStatus();
                    }), SlowDuration, false);

                UE_LOG(LogTemp, Log, TEXT("Applied slow effect to %s: Speed reduced to %f for %f seconds"),
                    *EnemyCharacter->GetName(), bIsFlyingEnemy ? MovementComponent->MaxFlySpeed : MovementComponent->MaxWalkSpeed, SlowDuration);
            }
        }
    }
}


