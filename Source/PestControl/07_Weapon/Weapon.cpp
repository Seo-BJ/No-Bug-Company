// Fill out your copyright notice in the Description page of Project Settings.



#include "07_Weapon/Weapon.h"
#include "07_Weapon/Projectile.h"

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

}

// Called when the game starts or when spawned
void AWeapon::BeginPlay()
{
	Super::BeginPlay();

    LoadWeaponStats(CurrentLevel);
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
        return;
    }

    if (!BulletSpawnPoint)
    {
        return;
    }

    FVector Location = BulletSpawnPoint->GetComponentLocation();
    FRotator Rotation = BulletSpawnPoint->GetComponentRotation();

    AProjectile* Projectile = GetWorld()->SpawnActor<AProjectile>(BulletClass, Location, Rotation);
    if (!Projectile)
    {
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

void AWeapon::ShotgunFire()
{
    if (!BulletClass || !BulletSpawnPoint)
    {
        return;
    }

    FVector SpawnLocation = BulletSpawnPoint->GetComponentLocation();
    FRotator BaseRotation = BulletSpawnPoint->GetComponentRotation();

    for (int32 i = 0; i < NumberOfProjectiles + 2; i++)
    {
        float RandomYaw = FMath::RandRange(-FireAngle, FireAngle);
        FRotator NewRotation = BaseRotation;
        NewRotation.Yaw += RandomYaw;

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
            NumberOfProjectiles = WeaponStats->NumberOfProjectiles;
            FireAngle = WeaponStats->FireAngle;
            RangeRadius = WeaponStats->RangeRadius;
            DurationTime = WeaponStats->DurationTime;

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

void AWeapon::InitInfo()
{
    InitialLocation = GetActorLocation();
    InitialRotation = GetActorRotation();
}

void AWeapon::ApplyDamageToEnemiesInRange()
{
    FVector WeaponLocation = InitialLocation;
    FRotator WeaponRotation = InitialRotation;

    float SphereRadius = RangeRadius + 50;

    TArray<FOverlapResult> OverlapResults;
    FCollisionShape CollisionSphere = FCollisionShape::MakeSphere(SphereRadius);

    bool bHasOverlaps = GetWorld()->OverlapMultiByChannel(
        OverlapResults,
        WeaponLocation,
        FQuat::Identity,
        ECC_Pawn,
        CollisionSphere
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

                FVector ForwardVector = WeaponRotation.Vector();
                float DotProduct = FVector::DotProduct(DirectionToEnemy, ForwardVector);

                if (DistanceToEnemy < 100.f || (FMath::Acos(DotProduct) * (180.f / PI)) <= FireAngle / 2.0f)
                {
                    APeCoEnemyCharacter* EnemyCharacter = Cast<APeCoEnemyCharacter>(OverlappedActor);
                    if (EnemyCharacter)
                    {
                        // 일반 대미지 적용
                        float ActualDamage = BaseDamage * DamageMultiplier;
                        if (FMath::RandRange(0.f, 1.f) < CriticalChance)
                        {
                            ActualDamage *= CriticalDamageMultiplier;
                        }

                        UGameplayStatics::ApplyDamage(EnemyCharacter, ActualDamage, GetInstigatorController(), this, UDamageType::StaticClass());

                        UE_LOG(LogTemp, Log, TEXT("Enemy %s hit by fan fire! Damage: %f"), *EnemyCharacter->GetName(), ActualDamage);

                        if (WeaponType == EWeaponType::Burn)
                        {
                            ApplyBurnDamage(EnemyCharacter);
                        }
                    }
                }
            }
        }
    }
}

void AWeapon::ApplyBurnDamage(APeCoEnemyCharacter* EnemyCharacter)
{
    if (IsValid(EnemyCharacter))
    {
        if (!EnemyCharacter->bIsBurning)
        {
            EnemyCharacter->bIsBurning = true;

            int32 NumTicks = FMath::FloorToInt(BurnDuration / BurnTickTime);
            for (int32 i = 0; i < NumTicks; i++)
            {
                if (IsValid(EnemyCharacter))  
                {
                    FTimerHandle BurnDamageHandle;
                    GetWorld()->GetTimerManager().SetTimer(BurnDamageHandle, FTimerDelegate::CreateLambda([=]()
                        {
                            if (IsValid(EnemyCharacter) && !EnemyCharacter->IsPendingKill())
                            {
                                UGameplayStatics::ApplyDamage(EnemyCharacter, BurnDamage, nullptr, this, nullptr);
                                UE_LOG(LogTemp, Warning, TEXT("Applying %f burn damage to %s"), BurnDamage, *EnemyCharacter->GetName());
                            }
                            else
                            {
                                return;
                            }
                        }), BurnTickTime * (i + 1), false);
                }
            }
            FTimerHandle ResetBurnHandle;
            GetWorld()->GetTimerManager().SetTimer(ResetBurnHandle, FTimerDelegate::CreateLambda([=]()
                {
                    if (IsValid(EnemyCharacter) && !EnemyCharacter->IsPendingKill())  
                    {
                        EnemyCharacter->ResetBurnStatus();
                    }
                    else
                    {
                        return;
                    }
                }), BurnDuration, false);
        }
        else
        {
            GetWorld()->GetTimerManager().ClearTimer(EnemyCharacter->ResetBurnHandle);
            GetWorld()->GetTimerManager().SetTimer(EnemyCharacter->ResetBurnHandle, FTimerDelegate::CreateLambda([=]()
                {
                    if (IsValid(EnemyCharacter) && !EnemyCharacter->IsPendingKill()) 
                    {
                        EnemyCharacter->ResetBurnStatus();
                    }
                }), BurnDuration, false);
        }
    }
}
