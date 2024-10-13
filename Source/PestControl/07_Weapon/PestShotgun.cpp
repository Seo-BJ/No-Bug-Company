// Fill out your copyright notice in the Description page of Project Settings.


#include "07_Weapon/PestShotgun.h"
#include "07_Weapon/Projectile.h"


APestShotgun::APestShotgun()
{
	WeaponID = FName(TEXT("PestShotgun"));

	CriticalChance = 0.05f;
	CriticalDamageMultiplier = 2.0f;
	BaseDamage = 15.f;
	DamageMultiplier = 1.0f;
	Cooldown = 0.7f;
	NumberOfProjectiles = 1;

	FireAngle = 30;
}

void APestShotgun::BeginPlay()
{
	Super::BeginPlay();

	SpreadRotation = BulletSpawnPoint->GetComponentRotation();

	if (bIsPestshogunValid)
	{
		GetWorld()->GetTimerManager().SetTimer(CooldownHandle, this, &APestShotgun::PestShotgunFire, Cooldown, true);
	}
}

void APestShotgun::PestShotgunFire()
{
	if (this)
	{
		AWeapon::ShotgunFire();
	}
}

