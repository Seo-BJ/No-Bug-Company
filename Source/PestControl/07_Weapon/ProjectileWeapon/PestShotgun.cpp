// Fill out your copyright notice in the Description page of Project Settings.


#include "07_Weapon/ProjectileWeapon/PestShotgun.h"

#include "01_Character/PeCoPlayerCharacter.h"

#include "Kismet/GameplayStatics.h"


APestShotgun::APestShotgun()
{
    WeaponTag = PeCoGameplayTags::Weapon_Projectile_PestShotgun;
    WeaponType = EWeaponType::Shotgun;
}

void APestShotgun::BeginPlay()
{
	Super::BeginPlay();
}

bool APestShotgun::EnhancementWeapon(int32 EnhancementIndex)
{
    if (Super::EnhancementWeapon(EnhancementIndex) == false) return false;
    if (EnhancementIndex == 0)
    {
        if (this)
        {
            this->NumberOfProjectiles++;
        }
        if (SecondShotgun)
        {
            SecondShotgun->NumberOfProjectiles++;
        }
    }
    return true;
}

bool APestShotgun::EvolveWeapon(int32 EvolveIndex)
{
    if (Super::EvolveWeapon(EvolveIndex) == false) return false;
    EvolvePestShotgun();
    return true;
}

void APestShotgun::EvolvePestShotgun()
{
    APeCoPlayerCharacter* PlayerCharacter = Cast<APeCoPlayerCharacter>(GetOwner());
    if (!PlayerCharacter || !PlayerCharacter->GetSecondWeaponSpawnPoint())
    {
        UE_LOG(LogTemp, Warning, TEXT("PlayerCharacter or SecondWeaponSpawnPoint is null."));
        return;
    }

    TSubclassOf<AWeapon> PestShotgunClass = PlayerCharacter->WeaponClassMap[PeCoGameplayTags::Weapon_Projectile_PestShotgun];
    if (!PestShotgunClass)
    {
        UE_LOG(LogTemp, Warning, TEXT("PestShotgunClass is invalid."));
        return;
    }

    FActorSpawnParameters SpawnParams;
    SpawnParams.Owner = PlayerCharacter;
    SpawnParams.Instigator = PlayerCharacter;

    FVector SpawnLocation = PlayerCharacter->GetSecondWeaponSpawnPoint()->GetComponentLocation();
    FRotator SpawnRotation = PlayerCharacter->GetSecondWeaponSpawnPoint()->GetComponentRotation();

    AWeapon* SpawnedWeapon = GetWorld()->SpawnActor<AWeapon>(PestShotgunClass, SpawnLocation, SpawnRotation, SpawnParams);

    SecondShotgun = SpawnedWeapon;

    if (SpawnedWeapon)
    {
        SpawnedWeapon->AttachToComponent(PlayerCharacter->GetSecondWeaponSpawnPoint(), FAttachmentTransformRules::SnapToTargetIncludingScale);
        UE_LOG(LogTemp, Log, TEXT("Spawned Level 1 PestShotgun at SecondWeaponSpawnPoint."));
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("Failed to spawn PestShotgun."));
    }

}