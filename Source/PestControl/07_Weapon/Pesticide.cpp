// Fill out your copyright notice in the Description page of Project Settings.


#include "07_Weapon/Pesticide.h"

#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h" 

APesticide::APesticide()
{
    WeaponID = FName(TEXT("Pesticide"));
    BaseDamage = 10.f;
    DamageMultiplier = 1.0f;
    CriticalChance = 0.1f;
    CriticalDamageMultiplier = 1.5f;
    Cooldown = 2.0f;
    FireAngle = 45.f; 
    RangeRadius = 350.f;  

    DurationTime = 1.f;

}

void APesticide::BeginPlay()
{
    Super::BeginPlay();
    UE_LOG(LogTemp, Warning, TEXT("Pesticide Weapon Spawned"));
    if (bIsPesticideValid)
    {
        GetWorld()->GetTimerManager().SetTimer(CooldownHandle, this, &APesticide::PesticideFire, Cooldown, true);
    }
}

void APesticide::PesticideFire()
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
        float Delay= FMath::Max(i * 0.25f, 0.25f);
        GetWorld()->GetTimerManager().SetTimer(TempHandle, this, &APesticide::DealDamage, Delay, false);
    }
 
        GetWorld()->GetTimerManager().SetTimer(CooldownHandle, this, &APesticide::StartCooldown, DurationTime, false);
}

void APesticide::DealDamage()
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
        FColor::Green,           
        false,                   
        0.2f
    );
}


void APesticide::StartCooldown()
{
    GetWorld()->GetTimerManager().SetTimer(CooldownHandle, this, &APesticide::PesticideFire, Cooldown, false);
}