// Fill out your copyright notice in the Description page of Project Settings.


#include "07_Weapon/ConicalWeapon/Pesticide.h"

#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h" 

APesticide::APesticide()
{
    WeaponTag == PeCoGameplayTags::Weapon_Conical_Pesticide;
    WeaponType = EWeaponType::Conical;
}

void APesticide::BeginPlay()
{
    Super::BeginPlay();


    DebugColor = FColor::Green;
}
