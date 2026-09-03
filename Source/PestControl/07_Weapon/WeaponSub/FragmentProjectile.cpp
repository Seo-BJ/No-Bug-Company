// Fill out your copyright notice in the Description page of Project Settings.

#include "07_Weapon/WeaponSub/FragmentProjectile.h"
#include "07_Weapon/ProjectileWeapon/WebRevolver.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/SphereComponent.h"
#include "Engine/World.h"
#include "TimerManager.h"
#include "Kismet/GameplayStatics.h"

AFragmentProjectile::AFragmentProjectile()
{
    PrimaryActorTick.bCanEverTick = true;

    CollisionComponent = CreateDefaultSubobject<USphereComponent>(TEXT("CollisionComponent"));
    CollisionComponent->InitSphereRadius(55.0f);
    CollisionComponent->SetCollisionProfileName(TEXT("OverlapAllDynamic"));
    CollisionComponent->SetCollisionObjectType(ECC_GameTraceChannel1);
    CollisionComponent->SetCollisionResponseToChannel(ECC_GameTraceChannel1, ECR_Ignore);
    CollisionComponent->SetupAttachment(ProjectileMesh);
}

void AFragmentProjectile::BeginPlay()
{
    Super::BeginPlay();

    CollisionComponent->SetCollisionObjectType(ECC_GameTraceChannel1);
    CollisionComponent->SetCollisionResponseToChannel(ECC_GameTraceChannel1, ECR_Ignore);

    // Overlap 델리게이트는 인스턴스당 1회만 바인딩.
    if (!bFragmentOverlapBound && CollisionComponent)
    {
        CollisionComponent->OnComponentBeginOverlap.AddDynamic(this, &AFragmentProjectile::OnOverlap);
        bFragmentOverlapBound = true;
    }

    // 최초 스폰 경로의 타이머 등록은 OnAcquired 경로에서 수행되므로 여기서는 생략.
    // (비 풀 경로로 BeginPlay가 먼저 불리는 경우 Super::BeginPlay가 OnAcquired_Implementation을
    //  호출해 StopMovement 타이머를 등록한다.)
}

void AFragmentProjectile::OnAcquired_Implementation(const FTransform& SpawnTransform, AActor* NewOwner, APawn* NewInstigator)
{
    Super::OnAcquired_Implementation(SpawnTransform, NewOwner, NewInstigator);

    CollisionComponent->SetCollisionObjectType(ECC_GameTraceChannel1);
    CollisionComponent->SetCollisionResponseToChannel(ECC_GameTraceChannel1, ECR_Ignore);

    // 풀에서 재사용될 때도 1회성 바인딩 보장.
    if (!bFragmentOverlapBound && CollisionComponent)
    {
        CollisionComponent->OnComponentBeginOverlap.AddDynamic(this, &AFragmentProjectile::OnOverlap);
        bFragmentOverlapBound = true;
    }

    bHasStopped = false;

    // 0.2초 후 비행 정지 - 재사용 시 타이머 재등록.
    if (UWorld* World = GetWorld())
    {
        World->GetTimerManager().ClearTimer(FlightTimerHandle);
        World->GetTimerManager().ClearTimer(DestructionTimerHandle);
        World->GetTimerManager().SetTimer(FlightTimerHandle, this, &AFragmentProjectile::StopMovement, 0.2f, false);
    }
}

void AFragmentProjectile::OnOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    if (AWebRevolver* Weapon = Cast<AWebRevolver>(GetOwner()))
    {
        if (APeCoEnemyCharacter* HitEnemy = Cast<APeCoEnemyCharacter>(OtherActor))
        {
            Weapon->ApplySlowEffect(HitEnemy, Weapon->WebBallSlowMultiplier);
        }
    }
}

void AFragmentProjectile::StopMovement()
{
    if (ProjectileMovementComponent)
    {
        ProjectileMovementComponent->StopMovementImmediately();
    }
    bHasStopped = true;

    GetWorld()->GetTimerManager().SetTimer(DestructionTimerHandle, this, &AFragmentProjectile::DestroyProjectile, WebDuration, false);
}

void AFragmentProjectile::DestroyProjectile()
{
    // 풀로 반환 (풀이 없으면 Destroy 폴백).
    ReleaseSelf();
}
