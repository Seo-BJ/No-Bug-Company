// Fill out your copyright notice in the Description page of Project Settings.


#include "07_Weapon/LarvaLauncher.h"

ALarvaLauncher::ALarvaLauncher()
{
    CriticalChance = 0.05f;
    CriticalDamageMultiplier = 1.5f;
    BaseDamage = 25.f;
    DamageMultiplier = 1.0f;
    Cooldown = 0.4f;
    NumberOfProjectiles = 1.f;
}

void ALarvaLauncher::BeginPlay()
{
    Super::BeginPlay();
    UE_LOG(LogTemp, Warning, TEXT("Larva Launcher Spawned"));
    if (bIsLarvaLauncherValid)
    {
        GetWorld()->GetTimerManager().SetTimer(CooldownHandle, this, &ALarvaLauncher::LarvaLauncherFire, Cooldown, true);
    }
}

void ALarvaLauncher::LarvaLauncherFire()
{
    for (int32 i = 1; i < NumberOfProjectiles + 1; i++)
    {
        FTimerHandle TempHandle;
        float Delay = FMath::Max(i * 0.1f, 0.1f);

        GetWorld()->GetTimerManager().SetTimer(TempHandle, this, &ALarvaLauncher::SpawnProjectiles, Delay, false);
    }
}

void ALarvaLauncher::SpawnProjectiles()
{
    if (this)
    {
        AWeapon::ProjectileFire();
    }
}