// Fill out your copyright notice in the Description page of Project Settings.



#include "Weapon.h"

#include "01_Character/PeCoEnemyCharacter.h" 
#include "01_Character/Components/InventoryComponent.h"
#include "04_UI/PeCoHUD.h"
#include "07_Weapon/Projectile.h"
#include "07_Weapon/ConicalWeapon/Flamethrower.h" 
#include "20_System/PeCoGameInstance.h"
#include "20_System/PeCoFunctionLibrary.h"
#include "21_Data/PeCoDataRow.h"

#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AWeapon::AWeapon()
{
	PrimaryActorTick.bCanEverTick = true;

    Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
    SetRootComponent(Root);

	WeaponMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("WeaponMesh"));
    WeaponMesh->SetupAttachment(RootComponent);

    BulletSpawnPoint = CreateDefaultSubobject<USceneComponent>(TEXT("Bullet Spawn Point"));
    BulletSpawnPoint->SetupAttachment(WeaponMesh);
}

void AWeapon::BeginPlay()
{
	Super::BeginPlay();
    InitWeaponData();
    GetWorld()->GetTimerManager().SetTimer(CooldownHandle, this, &AWeapon::FireWeapon, Cooldown, true);
}
void AWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AWeapon::InitWeaponData()
{
    UPeCoGameInstance* GameInstance = GetGameInstance<UPeCoGameInstance>();
    if (!IsValid(GameInstance))
    {
        return;
    }
    UDataTable* WeaponMaterialDataTable = GameInstance->WeaponInitInfoDataTable;
    if (!IsValid(WeaponMaterialDataTable))
    {
        UE_LOG(LogTemp, Error, TEXT("무기 강화 재료 데이터 테이블 없음."));
        return;
    }
    FName RowName = WeaponTag.GetTagName();
    FWeaponStats* RowData = WeaponMaterialDataTable->FindRow<FWeaponStats>(
        RowName,
        TEXT("Read Weapon Init Stat Data"),
        true
    );
    if (RowData == nullptr)
    {
        return;
    }
    BaseDamage = RowData->BaseDamage;
    DamageMultiplier = RowData->DamageMultiplier;
    CriticalChance = RowData->CriticalChance;
    CriticalDamageMultiplier = RowData->CriticalDamageMultiplier;
    Cooldown = RowData->Cooldown;
    Delay = RowData->Delay;
    NumberOfProjectiles = RowData->NumberOfProjectiles;
    Ammo = RowData->Ammo;
    MaxAmmo = RowData->Ammo;
    ReloadCoolDown = RowData->ReloadCoolDown;
    FireAngle = RowData->FireAngle;
    RangeRadius = RowData->RangeRadius;
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
       // UE_LOG(LogTemp, Warning, TEXT("Unknown weapon type"));
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
    // UE_LOG(LogTemp, Warning, TEXT("Reloading..."));
    GetWorld()->GetTimerManager().SetTimer(CooldownHandle, this, &AWeapon::Reload, ReloadCoolDown, false);
}

void AWeapon::Reload()
{
    Ammo = MaxAmmo;
    // UE_LOG(LogTemp, Log, TEXT("Reload complete. Ammo refilled to %d"), Ammo);
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

                        if (WeaponTag.MatchesTagExact(PeCoGameplayTags::Weapon_Conical_Flamethrower))
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




bool AWeapon::CanEnhancementWeapon()
{
    UPeCoGameInstance* GameInstance = GetGameInstance<UPeCoGameInstance>();
    if (!IsValid(GameInstance))
    {
        return false;
    }
    UDataTable* WeaponMaterialDataTable = *GameInstance->WeaponEnhancemenMaterialDataTableMap.Find(WeaponTag);
    if (!IsValid(WeaponMaterialDataTable))
    {
        UE_LOG(LogTemp, Error, TEXT("무기 강화 재료 데이터 테이블 없음."));
        return false;
    }
    AActor* OwnerCharacter = GetOwner();
    if (!IsValid(OwnerCharacter))
    {
        return false;
    }
    UInventoryComponent* InventoryComponent = UPeCoFunctionLibrary::GetInventoryComponent(OwnerCharacter);
    if (!IsValid(InventoryComponent))
    {
        return false;
    }
    FName RowName = FName(*FString::FromInt(EnhancementLevel+1));
    FWeaponEnhancementMaterialsData* RowData = WeaponMaterialDataTable->FindRow<FWeaponEnhancementMaterialsData>(
        RowName,
        TEXT("Read Weapon Enhancement Materials"), 
        true
    ); 

    return InventoryComponent->HasEnoughMaterials(RowData->RequiredMaterials);
}

bool AWeapon::CanEvolveWeapon()
{
    if (EnhancementLevel <= 4) return false;

    UPeCoGameInstance* GameInstance = GetGameInstance<UPeCoGameInstance>();
    if (!IsValid(GameInstance))
    {
        return false;
    }
    UDataTable* WeaponMaterialDataTable = *GameInstance->WeaponEnhancemenMaterialDataTableMap.Find(WeaponTag);
    if (!IsValid(WeaponMaterialDataTable))
    {
        UE_LOG(LogTemp, Error, TEXT("무기 강화 재료 데이터 테이블 없음."));
        return false;
    }
    AActor* OwnerCharacter = GetOwner();
    if (!IsValid(OwnerCharacter))
    {
        return false;
    }
    UInventoryComponent* InventoryComponent = UPeCoFunctionLibrary::GetInventoryComponent(OwnerCharacter);
    if (!IsValid(InventoryComponent))
    {
        return false;
    }
    FName RowName = FName(*FString::FromInt(EvolveLevel+1));
    FWeaponEnhancementMaterialsData* RowData = WeaponMaterialDataTable->FindRow<FWeaponEnhancementMaterialsData>(
        RowName,
        TEXT("Read Weapon Enhancement Materials"),
        true
    );

    return InventoryComponent->HasEnoughMaterials(RowData->RequiredMaterials);

}

bool AWeapon::EnhancementWeapon(int32 EnhancementIndex)
{
    if (!CanEnhancementWeapon())
    {
        return false;
    }
    EnhancementLevel += 1;
    return true;

}

bool AWeapon::EvolveWeapon(int32 EvolveIndex)
{
    if (!CanEvolveWeapon())
    {
        return false;
    }
    EvolveLevel += 1;
    return true;
}
