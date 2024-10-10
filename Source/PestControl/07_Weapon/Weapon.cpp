// Fill out your copyright notice in the Description page of Project Settings.

#include "07_Weapon/Weapon.h"
#include "07_Weapon/Projectile.h"

// Sets default values
AWeapon::AWeapon()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

    Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
    SetRootComponent(Root);

	WeaponMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("WeaponMesh"));
    WeaponMesh->SetupAttachment(RootComponent);

    BulletSpawnPoint = CreateDefaultSubobject<USceneComponent>(TEXT("Bullet Spawn Point"));
    BulletSpawnPoint->SetupAttachment(WeaponMesh);
}

// Called when the game starts or when spawned
void AWeapon::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void AWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AWeapon::ProjectileFire()
{
    if (!BulletClass)
    {
        UE_LOG(LogTemp, Error, TEXT("BulletClass is null. Cannot spawn projectile."));
        return;
    }

    if (!BulletSpawnPoint)
    {
        UE_LOG(LogTemp, Error, TEXT("BulletSpawnPoint is null. Cannot spawn projectile."));
        return;
    }

    FVector Location = BulletSpawnPoint->GetComponentLocation();
    FRotator Rotation = BulletSpawnPoint->GetComponentRotation();

    AProjectile* Projectile = GetWorld()->SpawnActor<AProjectile>(BulletClass, Location, Rotation);
    if (!Projectile)
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to spawn projectile."));
        return;
    }
 
    float ActualDamage = BaseDamage * DamageMultiplier;
    if (FMath::RandRange(0.f, 1.f) < CriticalChance)
    {
        ActualDamage *= CriticalDamageMultiplier;
    }

    Projectile->SetDamage(ActualDamage);
    Projectile->SetOwner(this);
}
