// Fill out your copyright notice in the Description page of Project Settings.



#include "Weapon.h"

#include "01_Character/PeCoEnemyCharacter.h" 
#include "01_Character/Components/InventoryComponent.h"

#include "04_UI/PeCoHUD.h"

#include "07_Weapon/Projectile.h"
#include "07_Weapon/ConicalWeapon/Flamethrower.h"

#include "20_System/PeCoGameInstance.h"
#include "20_System/PeCoFunctionLibrary.h"
#include "20_System/Pool/PeCoPoolSubsystem.h"

#include "21_Data/PeCoDataRow.h"

#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"

#include "Sound/SoundCue.h"

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

    WeaponStatLevelMap.Add(PeCoGameplayTags::WeaponStat_AttackPower, 0);
    WeaponStatLevelMap.Add(PeCoGameplayTags::WeaponStat_Damage, 0);
    WeaponStatLevelMap.Add(PeCoGameplayTags::WeaponStat_AttackSpeed, 0);
    WeaponStatLevelMap.Add(PeCoGameplayTags::WeaponStat_CriticalChance, 0);
    WeaponStatLevelMap.Add(PeCoGameplayTags::WeaponStat_CriticalDamage, 0);
    WeaponStatLevelMap.Add(PeCoGameplayTags::WeaponStat_Range, 0);
}

void AWeapon::BeginPlay()
{
    Super::BeginPlay();
    InitWeaponData();
    GetWorld()->GetTimerManager().SetTimer(CooldownHandle, this, &AWeapon::FireWeapon, GetActualCoolDown(), true);
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
        UE_LOG(LogTemp, Error, TEXT("Weapon Material Data Table 없음."));
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
    Range = RowData->Range;
}

bool AWeapon::GetCriticalDamage(float& OutDamage)
{
    if (FMath::RandRange(0.f, 1.f) <= CriticalChance)
    {
        OutDamage = GetActualDamage() * CriticalDamageMultiplier;
        return true;
    }
    else
    {
        OutDamage = GetActualDamage();
        return false;
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
        break;
    }

    if (FireSoundCue)
    {
        UGameplayStatics::PlaySoundAtLocation(this, FireSoundCue, GetActorLocation());
    }

    OnFire.Broadcast(Ammo, MaxAmmo);
    if (Ammo > 0)
    {
        GetWorld()->GetTimerManager().SetTimer(CooldownHandle, this, &AWeapon::FireWeapon, GetActualCoolDown(), false);
    }
    else
    {
        StartReload();
    }
}

void AWeapon::StartReload()
{
    bIsReloading = true;
    OnStartReload.Broadcast(ReloadCoolDown);
    GetWorld()->GetTimerManager().SetTimer(CooldownHandle, this, &AWeapon::Reload, ReloadCoolDown, false);
    if (ReloadSoundCue)
    {
        FTimerHandle SoundDelayHandle;
        GetWorld()->GetTimerManager().SetTimer(
            SoundDelayHandle,
            [this]()
            {
                UGameplayStatics::PlaySoundAtLocation(this, ReloadSoundCue, GetActorLocation());
            },
            0.2f, // 딜레이 시간 (초 단위)
            false
        );
    }
}

void AWeapon::Reload()
{
    bIsReloading = false;
    Ammo = MaxAmmo;
    // UE_LOG(LogTemp, Log, TEXT("Reload complete. Ammo refilled to %d"), Ammo);
    GetWorld()->GetTimerManager().SetTimer(CooldownHandle, this, &AWeapon::FireWeapon, GetActualCoolDown(), false);
}


void AWeapon::SpawnProjectile()
{
    if (!BulletClass || !BulletSpawnPoint)
    {
        return;
    }

    FVector Location = BulletSpawnPoint->GetComponentLocation();
    FRotator Rotation = BulletSpawnPoint->GetComponentRotation();

    APawn* InstigatorPawn = Cast<APawn>(GetOwner());
    AProjectile* Projectile = nullptr;

    if (UPeCoPoolSubsystem* Pool = GetWorld()->GetSubsystem<UPeCoPoolSubsystem>())
    {
        Projectile = Pool->Acquire<AProjectile>(BulletClass, FTransform(Rotation, Location), this, InstigatorPawn);
    }
    else
    {
        FActorSpawnParameters SpawnParams;
        SpawnParams.Owner = this;
        SpawnParams.Instigator = InstigatorPawn;
        Projectile = GetWorld()->SpawnActor<AProjectile>(BulletClass, Location, Rotation, SpawnParams);
    }

    if(Projectile)
    {
        float ActualDamage = 0.f;
        GetCriticalDamage(ActualDamage);

        Projectile->SetDamage(ActualDamage);
        Projectile->SetOwner(this);
    }
}
void AWeapon::ProjectileFire()
{
    const int32 ProjectileCount = FMath::Max(1, NumberOfProjectiles);
    const float ProjectileDelay = Delay;

    for (int32 i = 1; i <= ProjectileCount; i++)
    {
        FTimerHandle TempHandle;
        const float DelayTime = FMath::Max(i * ProjectileDelay, ProjectileDelay);

        GetWorld()->GetTimerManager().SetTimer(TempHandle, this, &AWeapon::SpawnProjectile, DelayTime, false);
    }
}

void AWeapon::ShotgunFire()
{
    if (!BulletClass || !BulletSpawnPoint)
    {
        return;
    }

    FVector SpawnLocation = BulletSpawnPoint->GetComponentLocation();
    FRotator BaseRotation = BulletSpawnPoint->GetComponentRotation();

    const int32 ProjectileCount = FMath::Max(1, NumberOfProjectiles);
    const float AngleIncrement = ProjectileCount > 1 ? FireAngle / (ProjectileCount - 1) : 0.0f;
    const float StartYaw = ProjectileCount > 1 ? BaseRotation.Yaw - (FireAngle / 2.0f) : BaseRotation.Yaw;

    APawn* InstigatorPawn = Cast<APawn>(GetOwner());
    UPeCoPoolSubsystem* Pool = GetWorld()->GetSubsystem<UPeCoPoolSubsystem>();

    for (int32 i = 0; i < ProjectileCount; i++)
    {
        FRotator NewRotation = BaseRotation;
        NewRotation.Yaw = StartYaw + i * AngleIncrement;

        AProjectile* Projectile = nullptr;
        if (Pool)
        {
            Projectile = Pool->Acquire<AProjectile>(BulletClass, FTransform(NewRotation, SpawnLocation), this, InstigatorPawn);
        }
        else
        {
            Projectile = GetWorld()->SpawnActor<AProjectile>(BulletClass, SpawnLocation, NewRotation);
        }

        if (Projectile)
        {
            // OnAcquired에서 이미 Velocity 재주입을 수행하지만, 샷건의 부채꼴 방향으로
            // 확실히 맞추기 위해 명시적으로 덮어쓴다.
            UProjectileMovementComponent* MovementComponent = Projectile->GetProjectileMovementComponent();

            if (MovementComponent)
            {
                FVector NewDirection = NewRotation.Vector();
                MovementComponent->Velocity = NewDirection * MovementComponent->InitialSpeed;
            }

            float ActualDamage = 0.f;
            GetCriticalDamage(ActualDamage);

            Projectile->SetDamage(ActualDamage);
            Projectile->SetOwner(this);
        }
    }
}
void AWeapon::StartShotgunCooldown()
{
    GetWorld()->GetTimerManager().SetTimer(CooldownHandle, this, &AWeapon::ShotgunFire, GetActualCoolDown(), false);
}

void AWeapon::ConicalFire()
{
    for (int32 i = 1; i < GetActualCoolDown() / Delay + 1 ; i++)
    {
        FTimerHandle TempHandle;
        float DelayTime = FMath::Max(i * Delay, Delay);
        GetWorld()->GetTimerManager().SetTimer(TempHandle, this, &AWeapon::DealDamageInSector, DelayTime, false);
    }
}

void AWeapon::DealDamageInSector()
{
    if (!GetWorld()) return;

    FVector Start = GetActorLocation();
    FVector ForwardVector = GetActorForwardVector();
    FVector End = Start + (ForwardVector * Range);

    CapsuleHalfHeight = Range * 0.5f;

    FCollisionShape CapsuleShape = FCollisionShape::MakeCapsule(CapsuleRadius, CapsuleHalfHeight);
    TArray<FHitResult> HitResults;

    FQuat CapsuleRotation = FRotationMatrix::MakeFromZ(ForwardVector).ToQuat();

    //DrawDebugCapsule(
    //    GetWorld(),
    //    Start + ForwardVector * (Range * 0.5f),
    //    CapsuleHalfHeight,
    //    CapsuleRadius,
    //    CapsuleRotation,
    //    FColor::Red,
    //    false,
    //    0.2f
    //);

    bool bHit = GetWorld()->SweepMultiByChannel(
        HitResults,
        Start,
        End,
        CapsuleRotation,
        ECC_Pawn,
        CapsuleShape
    );

    if (bHit)
    {
        for (const FHitResult& Hit : HitResults)
        {
            APeCoEnemyCharacter* Enemy = Cast<APeCoEnemyCharacter>(Hit.GetActor());
            if (IsValid(Enemy))
            {
                float ActualDamage = 0.0f;
                GetCriticalDamage(ActualDamage);
                UGameplayStatics::ApplyDamage(Enemy, ActualDamage, GetInstigatorController(), this, UDamageType::StaticClass());

                AFlamethrower* FlamethrowerWeapon = Cast<AFlamethrower>(this);
                if (FlamethrowerWeapon&& IsValid(Enemy))
                {
                    FlamethrowerWeapon->ApplyBurnEffect(Enemy);
                }
            }
        }
    }      
}




void AWeapon::UpgradeWeapon(FGameplayTag StatTag, float UpgradeAmount)
{
    if (StatTag.MatchesTagExact(PeCoGameplayTags::WeaponStat_AttackPower)) //피해량
    {
        DamageMultiplier += UpgradeAmount / 100;
    }
    else if (StatTag.MatchesTagExact(PeCoGameplayTags::WeaponStat_Damage)) //공격력
    {
        BaseDamage += UpgradeAmount;
    }
    else if (StatTag.MatchesTagExact(PeCoGameplayTags::WeaponStat_AttackSpeed))
    {
        CooldownMultiplier += UpgradeAmount / 100;
    }
    else if (StatTag.MatchesTagExact(PeCoGameplayTags::WeaponStat_CriticalChance))
    {
        CriticalChance += UpgradeAmount/100;
    }
    else if (StatTag.MatchesTagExact(PeCoGameplayTags::WeaponStat_CriticalDamage))
    {
        CriticalDamageMultiplier += UpgradeAmount / 100;
    }
    else if (StatTag.MatchesTagExact(PeCoGameplayTags::WeaponStat_Range))
    {
        Range += UpgradeAmount;
    }
    int32* Level = WeaponStatLevelMap.Find(StatTag);
    *Level += 1;
}

float AWeapon::GetStatValueByTag(FGameplayTag StatTag)
{
    if (StatTag.MatchesTagExact(PeCoGameplayTags::WeaponStat_Damage) || StatTag.MatchesTagExact(PeCoGameplayTags::WeaponStat_Damage)) //피해량
    {
        return GetActualDamage();
    }
    else if (StatTag.MatchesTagExact(PeCoGameplayTags::WeaponStat_AttackSpeed))
    {
        return GetAttackSpeedPerSecond();
    }
    else if (StatTag.MatchesTagExact(PeCoGameplayTags::WeaponStat_CriticalChance))
    {
        return CriticalChance;
    }
    else if (StatTag.MatchesTagExact(PeCoGameplayTags::WeaponStat_CriticalDamage))
    {
        return GetActualDamage() * CriticalDamageMultiplier;
    }
    else if (StatTag.MatchesTagExact(PeCoGameplayTags::WeaponStat_Range))
    {
        return Range;
    }
    else if (StatTag.MatchesTagExact(PeCoGameplayTags::WeaponStat_MaxAmmo))
    {
        return MaxAmmo;
    }

    return -1;
}




bool AWeapon::CanEnhancementWeapon()
{
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
    if (EnhancementLevel >= 5)
    {
        return false;
    }
     
    return InventoryComponent->HasEnoughMaterials(GetWeaponMaterialData(EnhancementLevel, true));
}
bool AWeapon::CanEvolveWeapon()
{
    if (EnhancementLevel <= 4) return false;
    if (EvolveLevel >= 1) return false;

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

    return InventoryComponent->HasEnoughMaterials(GetWeaponMaterialData(EvolveLevel, false));
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

TMap<FGameplayTag, int32> AWeapon::GetWeaponMaterialData(int32 CurrentLevel, bool bEnhancement)
{
    TMap<FGameplayTag, int32> MaterialMap;
    UPeCoGameInstance* GameInstance = GetGameInstance<UPeCoGameInstance>();
    if (!IsValid(GameInstance))
    {
        return MaterialMap;
    }
    UDataTable* WeaponMaterialDataTable = nullptr;
    if (bEnhancement)
    {
        WeaponMaterialDataTable = *GameInstance->WeaponEnhancemenMaterialDataTableMap.Find(WeaponTag);
        if (!IsValid(WeaponMaterialDataTable))
        {
            UE_LOG(LogTemp, Error, TEXT("Cant find Weapon Material Data"));
            return MaterialMap;
        }
        CurrentLevel = FMath::Clamp(CurrentLevel+1, 0, 5);
    }
    else
    {
        WeaponMaterialDataTable = *GameInstance->WeaponEvolveMaterialDataTableMap.Find(WeaponTag);
        if (!IsValid(WeaponMaterialDataTable))
        {
            UE_LOG(LogTemp, Error, TEXT("Cant find Weapon Material Data"));
            return MaterialMap;
        }
        CurrentLevel = FMath::Clamp(CurrentLevel+1, 0, 1);
    }

    AActor* OwnerCharacter = GetOwner();
    if (!IsValid(OwnerCharacter))
    {
        return MaterialMap;
    }
    UInventoryComponent* InventoryComponent = UPeCoFunctionLibrary::GetInventoryComponent(OwnerCharacter);
    if (!IsValid(InventoryComponent))
    {
        return MaterialMap;
    }
    FName RowName = FName(*FString::FromInt(CurrentLevel));
    FWeaponEnhancementMaterialsData* RowData = WeaponMaterialDataTable->FindRow<FWeaponEnhancementMaterialsData>(
        RowName,
        TEXT("Read Weapon Enhancement Materials"),
        true
    );
    MaterialMap = RowData->RequiredMaterials;
    return MaterialMap;
}
