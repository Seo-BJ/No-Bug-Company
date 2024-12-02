// Fill out your copyright notice in the Description page of Project Settings.



#include "07_Weapon/Weapon.h"
#include "07_Weapon/Projectile.h"
#include "07_Weapon/ConicalWeapon/Flamethrower.h" 

#include "01_Character/PeCoEnemyCharacter.h" 

#include "04_UI/PeCoHUD.h"

#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"

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

    CurrentLevel = 1;

    EnhencementLevel = 0;

}

// Called when the game starts or when spawned
void AWeapon::BeginPlay()
{
	Super::BeginPlay();

    LoadWeaponStats(CurrentLevel);
    
    GetWorld()->GetTimerManager().SetTimer(CooldownHandle, this, &AWeapon::FireWeapon, Cooldown, true);
}

// Called every frame
void AWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AWeapon::LoadWeaponStats(int32 Level)
{
    if (WeaponID.IsNone())
    {
        WeaponID = FName(TEXT("NoWeapon"));
    }
    FString RowNameString = FString::Printf(TEXT("%s_Lv%d"), *WeaponID.ToString(), Level);
    FName RowName = FName(*RowNameString);

    if (WeaponDataTable)
    {
        FWeaponStats* WeaponStats = WeaponDataTable->FindRow<FWeaponStats>(RowName, TEXT(""));
        if (WeaponStats)
        {
            BaseDamage = WeaponStats->BaseDamage;
            DamageMultiplier = WeaponStats->DamageMultiplier;
            CriticalChance = WeaponStats->CriticalChance;
            CriticalDamageMultiplier = WeaponStats->CriticalDamageMultiplier;
            Cooldown = WeaponStats->Cooldown;
            Delay = WeaponStats->Delay;
            NumberOfProjectiles = WeaponStats->NumberOfProjectiles;
            Ammo = WeaponStats->Ammo;
            MaxAmmo = WeaponStats->Ammo;
            ReloadCoolDown = WeaponStats->ReloadCoolDown;
            FireAngle = WeaponStats->FireAngle;
            RangeRadius = WeaponStats->RangeRadius;


            UE_LOG(LogTemp, Log, TEXT("Loaded stats for %s at Level %d"), *WeaponID.ToString(), Level);
        }
        else
        {
            UE_LOG(LogTemp, Warning, TEXT("Weapon stats not found in Data Table for %s at Level %d"), *WeaponID.ToString(), Level);
        }
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("WeaponDataTable is null"));
    }
}

void AWeapon::FireWeapon()
{
    if (Ammo <= 0)
    {
        StartReload();
        return;
    }

    switch (WeaponType)
    {
    case EWeaponType::Projectile:
        ProjectileFire();
        Ammo = Ammo - AmmoCost;
        break;

    case EWeaponType::Conical:
        ConicalFire();
        Ammo = Ammo - 25;
        break;

    case EWeaponType::Shotgun:
        ShotgunFire();
        Ammo--;
        break;

    default:
        UE_LOG(LogTemp, Warning, TEXT("Unknown weapon type"));
        break;
    }

    if (Ammo > 0)
    {
        GetWorld()->GetTimerManager().SetTimer(CooldownHandle, this, &AWeapon::FireWeapon, Cooldown, false);
    }
    else
    {
        StartReload();
    }
}

void AWeapon::StartReload()
{
    UE_LOG(LogTemp, Warning, TEXT("Reloading..."));

    GetWorld()->GetTimerManager().SetTimer(CooldownHandle, this, &AWeapon::Reload, ReloadCoolDown, false);
}

void AWeapon::Reload()
{
    Ammo = MaxAmmo;
    UE_LOG(LogTemp, Log, TEXT("Reload complete. Ammo refilled to %d"), Ammo);

    GetWorld()->GetTimerManager().SetTimer(CooldownHandle, this, &AWeapon::FireWeapon, Cooldown, false);
}


void AWeapon::SpawnProjectile()
{
    if (!BulletClass || !BulletSpawnPoint)
    {
        return;
    }

    FActorSpawnParameters SpawnParams;
    SpawnParams.Owner = this;
    SpawnParams.Instigator = Cast<APawn>(GetOwner());

    FVector Location = BulletSpawnPoint->GetComponentLocation();
    FRotator Rotation = BulletSpawnPoint->GetComponentRotation();

    AProjectile* Projectile = GetWorld()->SpawnActor<AProjectile>(BulletClass, Location, Rotation, SpawnParams);

    float ActualDamage = BaseDamage * DamageMultiplier ;
    if (FMath::RandRange(0.f, 1.f) < CriticalChance)
    {
        ActualDamage *= CriticalDamageMultiplier;
    }

    Projectile->SetDamage(ActualDamage);
    Projectile->SetOwner(this);

    //if (Projectile)
    //{
    //    UE_LOG(LogTemp, Log, TEXT("Projectile spawned with Owner: %s and Instigator: %s"),
    //        *Projectile->GetOwner()->GetName(),
    //        Projectile->GetInstigator() ? *Projectile->GetInstigator()->GetName() : TEXT("None"));
    //}
}

void AWeapon::ProjectileFire()
{
    for (int32 i = 1; i <= NumberOfProjectiles; i++)
    {
        FTimerHandle TempHandle;
        float DelayTime = FMath::Max(i * Delay, Delay);

        GetWorld()->GetTimerManager().SetTimer(TempHandle, this, &AWeapon::SpawnProjectile, DelayTime, false);
    }

    float TotalFireTime = FMath::Max(NumberOfProjectiles * Delay, Delay);
   
}


void AWeapon::ShotgunFire()
{
    if (!BulletClass || !BulletSpawnPoint)
    {
        return;
    }

    FVector SpawnLocation = BulletSpawnPoint->GetComponentLocation();
    FRotator BaseRotation = BulletSpawnPoint->GetComponentRotation();

    float AngleIncrement = FireAngle / (NumberOfProjectiles - 1);

    float StartYaw = BaseRotation.Yaw - (FireAngle / 2.0f);

    for (int32 i = 0; i < NumberOfProjectiles; i++)
    {
        FRotator NewRotation = BaseRotation;
        NewRotation.Yaw = StartYaw + i * AngleIncrement;

        AProjectile* Projectile = GetWorld()->SpawnActor<AProjectile>(BulletClass, SpawnLocation, NewRotation);

        if (Projectile)
        {
            UProjectileMovementComponent* MovementComponent = Projectile->GetProjectileMovementComponent();

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

            Projectile->SetDamage(ActualDamage);
            Projectile->SetOwner(this);
        }
    }
}

void AWeapon::StartShotgunCooldown()
{
    GetWorld()->GetTimerManager().SetTimer(CooldownHandle, this, &AWeapon::ShotgunFire, Cooldown, false);
}

void AWeapon::LevelUp()
{
    if (CurrentLevel < 3)
    {
        CurrentLevel++;
        LoadWeaponStats(CurrentLevel);
        UE_LOG(LogTemp, Log, TEXT("Weapon leveled up to Level %d"), CurrentLevel);
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("Max level reached! No more leveling up."));
    }
}

void AWeapon::Enhencement(int32 EnhencementIndex)
{
    ++EnhencementLevel;
}

void AWeapon::InitInfo()
{
    InitialLocation = GetActorLocation();
    InitialRotation = GetActorRotation();
}

void AWeapon::ConicalFire()
{
    //if (SprayEffect)
    //{
    //    UParticleSystemComponent* ParticleComp = UGameplayStatics::SpawnEmitterAtLocation(
    //        GetWorld(),
    //        SprayEffect,
    //        GetActorLocation(),
    //        GetActorRotation(),
    //        true
    //    );
    //    if (ParticleComp)
    //    {
    //        FTimerHandle ParticleTimerHandle;
    //        GetWorld()->GetTimerManager().SetTimer(ParticleTimerHandle, [ParticleComp]() { ParticleComp->DestroyComponent(); }, DurationTime + 0.5f, false);
    //    }
    //}

    InitInfo();

    for (int32 i = 1; i < Cooldown / Delay + 1 ; i++)
    {
        FTimerHandle TempHandle;
        float DelayTime = FMath::Max(i * Delay, Delay);
        GetWorld()->GetTimerManager().SetTimer(TempHandle, this, &AWeapon::DealDamageInSector, DelayTime, false);
    }
}

void AWeapon::DealDamageInSector()
{
    FVector WeaponLocation = GetActorLocation();
    FVector ForwardVector = GetActorRotation().Vector();

    float SectorRadius = RangeRadius;
    float HalfAngleRadians = FMath::DegreesToRadians(FireAngle / 2.0f);

    TArray<FOverlapResult> OverlapResults;
    FCollisionShape CollisionShape = FCollisionShape::MakeSphere(SectorRadius);

    bool bHasOverlaps = GetWorld()->OverlapMultiByChannel(
        OverlapResults,
        WeaponLocation,
        FQuat::Identity,
        ECC_Pawn,
        CollisionShape
    );

    DrawDebugCone(
        GetWorld(),
        WeaponLocation,
        ForwardVector,
        SectorRadius,
        HalfAngleRadians,
        HalfAngleRadians,
        12,
        DebugColor,
        false,
        0.2f
    );

    if (bHasOverlaps)
    {
        for (const FOverlapResult& OverlapResult : OverlapResults)
        {
            AActor* OverlappedActor = OverlapResult.GetActor();
            if (OverlappedActor && OverlappedActor->IsA(APeCoEnemyCharacter::StaticClass()))
            {
                FVector EnemyLocation = OverlappedActor->GetActorLocation();
                FVector DirectionToEnemy = EnemyLocation - WeaponLocation;
                float DistanceToEnemy = DirectionToEnemy.Size();
                DirectionToEnemy.Normalize();

                float DotProduct = FVector::DotProduct(DirectionToEnemy, ForwardVector);
                float AngleBetween = FMath::Acos(DotProduct) * (180.f / PI);

                if (DistanceToEnemy <= SectorRadius && AngleBetween <= FireAngle / 2.0f + 5.0f)
                {
                    APeCoEnemyCharacter* EnemyCharacter = Cast<APeCoEnemyCharacter>(OverlappedActor);
                    if (EnemyCharacter)
                    {
                        float ActualDamage = BaseDamage * DamageMultiplier;
                        if (FMath::RandRange(0.f, 1.f) < CriticalChance)
                        {
                            ActualDamage *= CriticalDamageMultiplier;
                        }

                        UGameplayStatics::ApplyDamage(EnemyCharacter, ActualDamage, GetInstigatorController(), this, UDamageType::StaticClass());

                        UE_LOG(LogTemp, Log, TEXT("Enemy %s hit by sector fire Damage: %f"), *EnemyCharacter->GetName(), ActualDamage);

                        if (WeaponID == FName("Flamethrower"))
                        {
                            AFlamethrower* Flamethrower = Cast<AFlamethrower>(this);
                            if (Flamethrower)
                            {
                                Flamethrower->ApplyBurnEffect(EnemyCharacter);
                            }
                        }
                    }
                }
            }
        }
    }
}