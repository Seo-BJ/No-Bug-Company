// Fill out your copyright notice in the Description page of Project Settings.


#include "07_Weapon/Pesticide.h"

#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h" 

APesticide::APesticide()
{
    WeaponID = FName(TEXT("Pesticide"));
  }

void APesticide::BeginPlay()
{
    Super::BeginPlay();
    WeaponType = EWeaponType::Conical;

    DebugColor = FColor::Green;
}
