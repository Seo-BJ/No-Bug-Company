// Fill out your copyright notice in the Description page of Project Settings.


#include "07_Weapon/ProjectileWeapon/LarvaLauncher.h"
#include "07_Weapon/Projectile.h"

#include "Kismet/GameplayStatics.h"

ALarvaLauncher::ALarvaLauncher()
{
    WeaponTag = PeCoGameplayTags::Weapon_Projectile_LarvaLauncher;
    WeaponType = EWeaponType::Projectile;

    EVBulletSpawnPoint = CreateDefaultSubobject<USceneComponent>(TEXT("Evolution Bullet Spawn Point"));
    EVBulletSpawnPoint->SetupAttachment(WeaponMesh);

}
void ALarvaLauncher::BeginPlay()
{
    Super::BeginPlay();
}

void ALarvaLauncher::ApplyWitherEffect(APeCoEnemyCharacter* EnemyCharacter)
{
    if (!EnemyCharacter)
    {
        return;
    }

    if (!EnemyCharacter->bIsWithered)
    {
        EnemyCharacter->bIsWithered = true;

        AController* InstigatorController = nullptr;

        if (APawn* OwnerPawn = Cast<APawn>(GetOwner()))
        {
            InstigatorController = OwnerPawn->GetController();
        }
        EnemyCharacter->ApplyTickDamage(TickInterval, WitherDamage, WitherDuration, this, InstigatorController);
    };
}

void ALarvaLauncher::SpawnProjectile()
{
    Super::SpawnProjectile(); 

    if (bIsFirstEvolved && EVBulletSpawnPoint && BulletClass)
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



bool ALarvaLauncher::EnhancementWeapon(int32 EnhancementIndex)
{
    if (Super::EnhancementWeapon(EnhancementIndex) == false) return false;
    WitherDamage += 5;
    return true;
}

bool ALarvaLauncher::EvolveWeapon(int32 EvolveIndex)
{
    if (Super::EvolveWeapon(EvolveIndex) == false) return false;
    LarvaLauncherEvolve();
    return true;
}

void ALarvaLauncher::LarvaLauncherEvolve()
{
    if (EvolveLevel < 2)
    {
        if (EvolveLevel == 0)
        {
            FirstEvolveLarvaLauncher();
        }
        else
        {
            SecondEvolveLarvaLauncher();
        }
    }
    else
    {
        return;
    }
}
void ALarvaLauncher::FirstEvolveLarvaLauncher()
{
    bIsFirstEvolved = true;

    if (BulletSpawnPoint && EVBulletSpawnPoint)
    {
        FVector Offset(0.f, 50.f, 0.f);
        EVBulletSpawnPoint->SetRelativeLocation(BulletSpawnPoint->GetRelativeLocation() + Offset);
    }

    UE_LOG(LogTemp, Log, TEXT("LarvaLauncher has evolved! EVBulletSpawnPoint is now active."));
}

void ALarvaLauncher::SecondEvolveLarvaLauncher()
{
    if (bIsFirstEvolved)
    {
        BaseDamage = BaseDamage + SecondEvolveDamage;
    }
    else
    {
        return;
    }
}