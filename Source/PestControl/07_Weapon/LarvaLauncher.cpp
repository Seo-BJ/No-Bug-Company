// Fill out your copyright notice in the Description page of Project Settings.


#include "07_Weapon/LarvaLauncher.h"

ALarvaLauncher::ALarvaLauncher()
{
    WeaponID = FName(TEXT("LarvaLauncher"));

    CriticalChance = 0.05f;
    CriticalDamageMultiplier = 1.5f;
    BaseDamage = 25.f;
    DamageMultiplier = 1.0f;
    Cooldown = 0.4f;
    NumberOfProjectiles = 1;
}

void ALarvaLauncher::BeginPlay()
{
    Super::BeginPlay();
    if (bIsLarvaLauncherValid)
    {
        GetWorld()->GetTimerManager().SetTimer(CooldownHandle, this, &ALarvaLauncher::LarvaLauncherFire, Cooldown, true);
    }
}

void ALarvaLauncher::LarvaLauncherFire()
{
    for (int32 i = 1; i <= NumberOfProjectiles; i++)  
    {
        FTimerHandle TempHandle;
        float Delay = FMath::Max(i * 0.1f, 0.1f);

        GetWorld()->GetTimerManager().SetTimer(TempHandle, this, &ALarvaLauncher::SpawnProjectiles, Delay, false);
    }

    float TotalFireTime = FMath::Max(NumberOfProjectiles * 0.1f, 0.1f);  
    GetWorld()->GetTimerManager().SetTimer(CooldownHandle, this, &ALarvaLauncher::StartCooldown, TotalFireTime, false);
}

void ALarvaLauncher::StartCooldown()
{
    GetWorld()->GetTimerManager().SetTimer(CooldownHandle, this, &ALarvaLauncher::LarvaLauncherFire, Cooldown, false);
}

void ALarvaLauncher::SpawnProjectiles()
{
    if (this)
    {
        AWeapon::ProjectileFire();
    }
}