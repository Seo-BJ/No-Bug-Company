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
    if (!EnemyCharacter || !GetWorld())
    {
        return;
    }

    if (WitherTimers.Contains(EnemyCharacter))
    {
        UE_LOG(LogTemp, Warning, TEXT("%s는 이미 Wither 상태입니다."), *EnemyCharacter->GetName());
        return;
    }

    FTimerHandle WitherTimerHandle;
    GetWorld()->GetTimerManager().SetTimer(WitherTimerHandle, FTimerDelegate::CreateLambda([=]()
        {
            if (IsValid(EnemyCharacter))
            {
                UGameplayStatics::ApplyDamage(
                    EnemyCharacter,
                    WitherDamage,
                    GetInstigatorController(),
                    this,
                    UDamageType::StaticClass()
                );

                UE_LOG(LogTemp, Warning, TEXT("Withered 상태: %s에게 %f 데미지 적용"), *EnemyCharacter->GetName(), WitherDamage);
            }
            else
            {
                ClearWitherEffect(EnemyCharacter);
            }
        }), TickInterval, true);

    WitherTimers.Add(EnemyCharacter, WitherTimerHandle);

    FTimerHandle EndWitherHandle;
    GetWorld()->GetTimerManager().SetTimer(EndWitherHandle, FTimerDelegate::CreateLambda([=]()
        {
            ClearWitherEffect(EnemyCharacter);

            UE_LOG(LogTemp, Warning, TEXT("%s Withered 상태 해제"), *EnemyCharacter->GetName());
        }), WitherDuration, false);

    EndWitherTimers.Add(EnemyCharacter, EndWitherHandle);
}

void ALarvaLauncher::ClearWitherEffect(APeCoEnemyCharacter* EnemyCharacter)
{
    if (!EnemyCharacter)
    {
        return;
    }

    if (WitherTimers.Contains(EnemyCharacter))
    {
        FTimerHandle& TimerHandle = WitherTimers[EnemyCharacter];
        GetWorld()->GetTimerManager().ClearTimer(TimerHandle);
        WitherTimers.Remove(EnemyCharacter);
    }

    if (EndWitherTimers.Contains(EnemyCharacter))
    {
        FTimerHandle& TimerHandle = EndWitherTimers[EnemyCharacter];
        GetWorld()->GetTimerManager().ClearTimer(TimerHandle);
        EndWitherTimers.Remove(EnemyCharacter);
    }

    UE_LOG(LogTemp, Log, TEXT("%s의 Wither 상태가 종료되었습니다."), *EnemyCharacter->GetName());
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
       
        float ActualDamage = BaseDamage * DamageMultiplier;
        if (FMath::RandRange(0.f, 1.f) < CriticalChance)
        {
            ActualDamage *= CriticalDamageMultiplier;
        }

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