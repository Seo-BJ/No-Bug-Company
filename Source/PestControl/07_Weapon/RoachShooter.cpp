// Fill out your copyright notice in the Description page of Project Settings.


#include "07_Weapon/RoachShooter.h"

ARoachShooter::ARoachShooter()
{
    WeaponID = FName(TEXT("RoachShooter"));
}

void ARoachShooter::BeginPlay()
{
    Super::BeginPlay();
    WeaponType = EWeaponType::Projectile;
}
