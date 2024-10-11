// Fill out your copyright notice in the Description page of Project Settings.


#include "07_Weapon/AirGun.h"

AAirGun::AAirGun()
{
    WeaponID = FName(TEXT("AirGun"));

    CriticalChance = 0.05f;
    CriticalDamageMultiplier = 1.5f;
    BaseDamage = 85.f;
    DamageMultiplier = 1.0f;
    Cooldown = 0.5f;
    NumberOfProjectiles = 1;
}

void AAirGun::BeginPlay()
{
    Super::BeginPlay();
    if (bIsAirGunValid)
    {
        GetWorld()->GetTimerManager().SetTimer(CooldownHandle, this, &AAirGun::AirGunFire, Cooldown, true);
    }
}

void AAirGun::AirGunFire()
{
    for (int32 i = 1; i <= NumberOfProjectiles; i++)
    {
        FTimerHandle TempHandle;
        float Delay = FMath::Max(i * 0.1f, 0.1f);

        GetWorld()->GetTimerManager().SetTimer(TempHandle, this, &AAirGun::SpawnProjectiles, Delay, false);
    }

    float TotalFireTime = FMath::Max(NumberOfProjectiles * 0.1f, 0.1f);
    GetWorld()->GetTimerManager().SetTimer(CooldownHandle, this, &AAirGun::StartCooldown, TotalFireTime, false);
}

void AAirGun::StartCooldown()
{
    GetWorld()->GetTimerManager().SetTimer(CooldownHandle, this, &AAirGun::AirGunFire, Cooldown, false);
}

void AAirGun::SpawnProjectiles()
{
    if (this)
    {
        AWeapon::ProjectileFire();
    }
}