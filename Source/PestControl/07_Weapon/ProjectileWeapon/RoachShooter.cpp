// Fill out your copyright notice in the Description page of Project Settings.


#include "07_Weapon/ProjectileWeapon/RoachShooter.h"
#include "07_Weapon/Projectile.h"

#include "01_Character/PeCoPlayerCharacter.h"

ARoachShooter::ARoachShooter()
{
    WeaponTag = PeCoGameplayTags::Weapon_Projectile_RoachShooter;
    WeaponType = EWeaponType::Projectile;

    EVBulletSpawnPoint = CreateDefaultSubobject<USceneComponent>(TEXT("Evolution Bullet Spawn Point"));
    EVBulletSpawnPoint->SetupAttachment(WeaponMesh);
}

void ARoachShooter::BeginPlay()
{
    Super::BeginPlay();

}

void ARoachShooter::SpawnProjectile()
{
    Super::SpawnProjectile();

    if (HasWeaponEvolved() && EVBulletSpawnPoint && BulletClass)
    {
        FActorSpawnParameters SpawnParams;
        SpawnParams.Owner = this;
        SpawnParams.Instigator = Cast<APawn>(GetOwner());

        FVector EVLocation = EVBulletSpawnPoint->GetComponentLocation();
        FRotator EVRotation = EVBulletSpawnPoint->GetComponentRotation();

        AProjectile* EVProjectile = GetWorld()->SpawnActor<AProjectile>(BulletClass, EVLocation, EVRotation, SpawnParams);

        float ActualDamage = 0.f;
        GetCriticalDamage(ActualDamage);

        EVProjectile->SetDamage(ActualDamage);
        EVProjectile->SetOwner(this);

    }
}

void ARoachShooter::EvolveRoachShooter()
{
    if (BulletSpawnPoint && EVBulletSpawnPoint)
    {
        FVector Offset(0.f, 50.f, 0.f);
        EVBulletSpawnPoint->SetRelativeLocation(BulletSpawnPoint->GetRelativeLocation() + Offset);
    }

    DamageMultiplier = 0.75;
    AmmoCost = 2;

    APawn* OwnerPawn = Cast<APawn>(GetOwner());
    if (OwnerPawn)
    {
        ACharacter* OwnerCharacter = Cast<APeCoPlayerCharacter>(OwnerPawn);

        if (OwnerCharacter && OwnerCharacter->GetCharacterMovement())
        {
            UCharacterMovementComponent* MovementComponent = OwnerCharacter->GetCharacterMovement();
            float CurrentSpeed = MovementComponent->MaxWalkSpeed;
            MovementComponent->MaxWalkSpeed = CurrentSpeed * 0.9f;

            UE_LOG(LogTemp, Log, TEXT("RoachShooter evolved! Owner's movement speed decreased by 10%%. New Speed: %f"), MovementComponent->MaxWalkSpeed);
        }
        else
        {
            UE_LOG(LogTemp, Warning, TEXT("OwnerCharacter or CharacterMovementComponent is invalid."));
        }
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("Owner is not a valid APawn."));
    }
}

bool ARoachShooter::EnhancementWeapon(int32 EnhancementIndex)
{
    if (Super::EnhancementWeapon(EnhancementIndex) == false) return false;
    Ammo += 10;
    return true;
}

bool ARoachShooter::EvolveWeapon(int32 EvolveIndex)
{
    if (Super::EvolveWeapon(EvolveIndex) == false) return false;
    EvolveRoachShooter();
    return true;

}
