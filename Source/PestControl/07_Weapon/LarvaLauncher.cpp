// Fill out your copyright notice in the Description page of Project Settings.


#include "07_Weapon/LarvaLauncher.h"

ALarvaLauncher::ALarvaLauncher()
{
    WeaponID = FName(TEXT("LarvaLauncher"));
}

void ALarvaLauncher::BeginPlay()
{
    Super::BeginPlay();
    WeaponType = EWeaponType::Projectile;
}