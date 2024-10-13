// Fill out your copyright notice in the Description page of Project Settings.


#include "07_Weapon/RoachShooter.h"

ARoachShooter::ARoachShooter()
{
    WeaponID = FName(TEXT("RoachShooter"));

    CriticalChance = 0.05f;
    CriticalDamageMultiplier = 1.5f;
    BaseDamage = 100.f;
    DamageMultiplier = 1.0f;
    Cooldown = 0.1f;
    NumberOfProjectiles = 1;
}

void ARoachShooter::BeginPlay()
{
    Super::BeginPlay();
 
    if (bIsRoachShooterValid)
    {
        GetWorld()->GetTimerManager().SetTimer(CooldownHandle, this, &ARoachShooter::SpawnProjectiles, Cooldown, true);
    }
}

void ARoachShooter::RoachShooterFire()
{
}

void ARoachShooter::SpawnProjectiles()
{
    if (this)
    {
        AWeapon::ProjectileFire();
    }
}