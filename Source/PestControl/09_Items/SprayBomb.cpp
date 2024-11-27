// Fill out your copyright notice in the Description page of Project Settings.


#include "09_Items/SprayBomb.h"

#include "01_Character/PeCoEnemyCharacter.h"
#include "01_Character/PeCoPlayerCharacter.h"

#include "02_Player/PeCoPlayerController.h"

#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"

#include "Kismet/GameplayStatics.h"

// Sets default values
ASprayBomb::ASprayBomb()
{
	PrimaryActorTick.bCanEverTick = false;

    BombMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BombMesh"));
    RootComponent = BombMesh;

    BombMesh->SetSimulatePhysics(true);
    BombMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);

    ExplodeRange = CreateDefaultSubobject<USphereComponent>(TEXT("DetectionSphere"));
    ExplodeRange->SetupAttachment(BombMesh);
    
}
void ASprayBomb::UseCombatItem()
{
    Super::UseCombatItem();
    ThrowGrenade();
}

void ASprayBomb::BeginPlay()
{
	Super::BeginPlay();

    if (IsValid(this) && IsValid(BombMesh))
    {
        BombMesh->OnComponentHit.AddDynamic(this, &ASprayBomb::OnHit);
    }
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

void ASprayBomb::ThrowGrenade()
{
    if (!GetOwner())
    {
        return;
    }
    APeCoPlayerCharacter* PlayerCharacter = CastChecked<APeCoPlayerCharacter>(GetOwner());

    FVector StartLocation = GetActorLocation();
    FVector TargetLocation = GetCursorLocation();
    float arcValue = 0.5f;                       // ArcParam (0.0-1.0)
    FVector outVelocity = FVector::ZeroVector;

    float Distance = FVector::Dist(StartLocation, TargetLocation);

    // BombRange보다 먼 경우, BombRange 내의 위치로 TargetLocation을 설정
    if (Distance > BombRange)
    {
        FVector Direction = (TargetLocation - StartLocation).GetSafeNormal();
        TargetLocation = StartLocation + Direction * BombRange;
    }

    if (UGameplayStatics::SuggestProjectileVelocity_CustomArc(this, outVelocity, StartLocation, TargetLocation, GetWorld()->GetGravityZ(), arcValue))
    {
        FPredictProjectilePathParams predictParams(20.0f, StartLocation, outVelocity, 1.0f);
        predictParams.DrawDebugType = EDrawDebugTrace::Type::ForDuration;
        predictParams.OverrideGravityZ = GetWorld()->GetGravityZ();
        FPredictProjectilePathResult result;
        UGameplayStatics::PredictProjectilePath(this, predictParams, result);

        if (IsValid(BombMesh))
        {
            BombMesh->AddImpulse(outVelocity, NAME_None, true);
        }
    }
}

FVector ASprayBomb::GetCursorLocation()
{
    if (!IsValid(GetOwner()))
    {
        UE_LOG(LogTemp, Warning, TEXT("스프레이 폭탄의 Owner가 유효하지 않음!"));
        return FVector();
    }
    APeCoPlayerController* PlayerController = GetOwner()->GetInstigatorController<APeCoPlayerController>();
    if (!IsValid(PlayerController))
    {
        UE_LOG(LogTemp, Warning, TEXT("스프레이 폭탄의 Owner의 컨트롤러가 유효하지 않음!"));
        return FVector();
    }
    FHitResult HitResult;
    PlayerController->GetHitResultUnderCursor(
        ECollisionChannel::ECC_WorldStatic,
        false,
        HitResult
    );
    return HitResult.Location;
}

