// Fill out your copyright notice in the Description page of Project Settings.


#include "07_Weapon/PestShotgun.h"
#include "07_Weapon/Projectile.h"


APestShotgun::APestShotgun()
{
	WeaponID = FName(TEXT("PestShotgun"));
}

void APestShotgun::BeginPlay()
{
	Super::BeginPlay();

	WeaponType = EWeaponType::Shotgun;
}