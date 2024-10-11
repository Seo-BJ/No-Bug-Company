// Fill out your copyright notice in the Description page of Project Settings.


#include "07_Weapon/Flamethrower.h"

#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h" 


AFlamethrower::AFlamethrower()
{
    WeaponID = FName(TEXT("Flamethrower"));
    BaseDamage = 115.f;
    DamageMultiplier = 1.0f;
    CriticalChance = 0.05f;
    CriticalDamageMultiplier = 1.5f;
    Cooldown = 2.0f;
    FireAngle = 20.f;
    RangeRadius = 350.f;

    DurationTime = 1.f;
}

void AFlamethrower::BeginPlay()
{
    Super::BeginPlay();
    UE_LOG(LogTemp, Warning, TEXT("Flamethrower Weapon Spawned"));
    if (bIsFlamethrowerValid)
    {
        GetWorld()->GetTimerManager().SetTimer(CooldownHandle, this, &AFlamethrower::FlamethrowerFire, Cooldown, true);
    }
}

void AFlamethrower::FlamethrowerFire()
{
    if (SprayEffect)
    {
        UParticleSystemComponent* ParticleComp = UGameplayStatics::SpawnEmitterAtLocation(
            GetWorld(),
            SprayEffect,
            GetActorLocation(),
            GetActorRotation(),
            true
        );
        if (ParticleComp)
        {
            FTimerHandle ParticleTimerHandle;
            GetWorld()->GetTimerManager().SetTimer(
                ParticleTimerHandle,
                [ParticleComp]()
                {
                    ParticleComp->Deactivate();
                },
                DurationTime,
                false
            );
        }
    }

    if (SprayEffect)
    {
        UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), SprayEffect, GetActorLocation(), GetActorRotation(), true);
    }

    InitInfo();

    for (int32 i = 1; i < DurationTime / TickTime + 1; i++)
    {
        FTimerHandle TempHandle;
        float Delay = FMath::Max(i * 0.25f, 0.25f); 
        GetWorld()->GetTimerManager().SetTimer(TempHandle, this, &AFlamethrower::DealDamage, Delay, false);
    }

    GetWorld()->GetTimerManager().SetTimer(CooldownHandle, this, &AFlamethrower::StartCooldown, DurationTime, false);
}

void AFlamethrower::DealDamage()
{
    ApplyDamageToEnemiesInRange();

    FVector WeaponLocation = InitialLocation;
    FVector ForwardVector = InitialRotation.Vector();

    float ConeRadius = RangeRadius + 50;
    float HalfAngleRadians = FMath::DegreesToRadians(FireAngle / 2.0f);

    DrawDebugCone(
        GetWorld(),
        WeaponLocation,       
        ForwardVector,           
        ConeRadius,       
        HalfAngleRadians,    
        HalfAngleRadians,
        12,
        FColor::Red,           
        false,                   
        0.2f
    );
}

void AFlamethrower::StartCooldown()
{
    GetWorld()->GetTimerManager().SetTimer(CooldownHandle, this, &AFlamethrower::FlamethrowerFire, Cooldown, false);
}
