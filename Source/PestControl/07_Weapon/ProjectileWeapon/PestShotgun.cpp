// Fill out your copyright notice in the Description page of Project Settings.


#include "07_Weapon/ProjectileWeapon/PestShotgun.h"


APestShotgun::APestShotgun()
{
	WeaponID = FName(TEXT("PestShotgun"));
}

void APestShotgun::BeginPlay()
{
	Super::BeginPlay();

	WeaponType = EWeaponType::Shotgun;
}