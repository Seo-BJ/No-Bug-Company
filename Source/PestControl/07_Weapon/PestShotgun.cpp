// Fill out your copyright notice in the Description page of Project Settings.

#include "GameFramework/ProjectileMovementComponent.h"

#include "07_Weapon/Projectile.h"
#include "07_Weapon/PestShotgun.h"

APestShotgun::APestShotgun()
{
	CriticalChance = 0.05f;
	CriticalDamageMultiplier = 2.0f;
	BaseDamage = 15.f;
	DamageMultiplier = 1.0f;
	Cooldown = 0.7f;
	NumberOfProjectiles = 1;

    SpreadAngle = 30.f;
}

void APestShotgun::BeginPlay()
{
	Super::BeginPlay();
	UE_LOG(LogTemp, Warning, TEXT("PestShotgun Spawned"));

	SpreadRotation = BulletSpawnPoint->GetComponentRotation();

	if (bIsPestshogunValid)
	{
		GetWorld()->GetTimerManager().SetTimer(CooldownHandle, this, &APestShotgun::PestShotgunFire, Cooldown, true);
	}
}

void APestShotgun::PestShotgunFire()
{
    if (!BulletClass || !BulletSpawnPoint)
    {
        UE_LOG(LogTemp, Error, TEXT("BulletClass or BulletSpawnPoint is null. Cannot fire projectiles."));
        return;
    }

    FVector SpawnLocation = BulletSpawnPoint->GetComponentLocation();
    FRotator BaseRotation = BulletSpawnPoint->GetComponentRotation();

    for (int32 i = 0; i < NumberOfProjectiles + 4; i++)
    {
        float RandomYaw = FMath::RandRange(-SpreadAngle, SpreadAngle);
        FRotator NewRotation = BaseRotation;
        NewRotation.Yaw += RandomYaw;

        AProjectile* SpawnedProjectile = GetWorld()->SpawnActor<AProjectile>(BulletClass, SpawnLocation, NewRotation);

        if (SpawnedProjectile)
        {
            UProjectileMovementComponent* MovementComponent = SpawnedProjectile->GetProjectileMovementComponent();

            if (MovementComponent)
            {
                FVector NewDirection = NewRotation.Vector();
                MovementComponent->Velocity = NewDirection * MovementComponent->InitialSpeed;
            }

            float ActualDamage = BaseDamage * DamageMultiplier;
            if (FMath::RandRange(0.f, 1.f) < CriticalChance)
            {
                ActualDamage *= CriticalDamageMultiplier;
            }

            SpawnedProjectile->SetDamage(ActualDamage);
            SpawnedProjectile->SetOwner(this);
        }
        else
        {
            UE_LOG(LogTemp, Error, TEXT("Failed to spawn projectile."));
        }
    }
}

void APestShotgun::SpawnProjectiles()
{
	if (this)
	{
		AWeapon::ProjectileFire();
	}
}
