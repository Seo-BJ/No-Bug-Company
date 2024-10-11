// Fill out your copyright notice in the Description page of Project Settings.


#include "07_Weapon/WebRevolver.h"

AWebRevolver::AWebRevolver()
{
    WeaponID = FName(TEXT("WebRevolver"));

    CriticalChance = 0.05f;
    CriticalDamageMultiplier = 1.5f;
    BaseDamage = 50.f;
    DamageMultiplier = 1.0f;
    Cooldown = 1.0f;
    NumberOfProjectiles = 1;
}

void AWebRevolver::BeginPlay()
{
    Super::BeginPlay();
    if (bIsWebRevolverValid)
    {
        GetWorld()->GetTimerManager().SetTimer(CooldownHandle, this, &AWebRevolver::WebRevolverFire, Cooldown, true);
    }
}

void AWebRevolver::WebRevolverFire()
{
    for (int32 i = 1; i < NumberOfProjectiles + BaseAmmo; i++)
    {
        FTimerHandle TempHandle;
        float Delay = FMath::Max(i * 0.2f, 0.2f);  

        GetWorld()->GetTimerManager().SetTimer(TempHandle, this, &AWebRevolver::SpawnProjectiles, Delay, false);
    }

    float TotalFireTime = FMath::Max((NumberOfProjectiles + BaseAmmo - 1 ) * 0.2f, 0.2f);
    GetWorld()->GetTimerManager().SetTimer(CooldownHandle, this, &AWebRevolver::StartCooldown, TotalFireTime, false);
}

void AWebRevolver::StartCooldown()
{
    GetWorld()->GetTimerManager().SetTimer(CooldownHandle, this, &AWebRevolver::WebRevolverFire, Cooldown, false);
}


void AWebRevolver::SpawnProjectiles()
{
    if (this)
    {
        AWeapon::ProjectileFire();
    }
}
