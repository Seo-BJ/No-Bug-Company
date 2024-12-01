// Fill out your copyright notice in the Description page of Project Settings.

#include "07_Weapon/WeaponSub/FragmentProjectile.h"
#include "07_Weapon/ProjectileWeapon/WebRevolver.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/SphereComponent.h"
#include "Kismet/GameplayStatics.h"

AFragmentProjectile::AFragmentProjectile()
{
    PrimaryActorTick.bCanEverTick = true;

    CollisionComponent = CreateDefaultSubobject<USphereComponent>(TEXT("CollisionComponent"));
    CollisionComponent->InitSphereRadius(55.0f); 
    CollisionComponent->SetCollisionProfileName(TEXT("OverlapAllDynamic")); 
    CollisionComponent->SetupAttachment(ProjectileMesh);
}

void AFragmentProjectile::BeginPlay()
{
    Super::BeginPlay();

    CollisionComponent ->OnComponentBeginOverlap.AddDynamic(this, &AFragmentProjectile::OnOverlap);

    GetWorld()->GetTimerManager().SetTimer(FlightTimerHandle, this, &AFragmentProjectile::StopMovement, 0.2f, false);
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
    Destroy();
}