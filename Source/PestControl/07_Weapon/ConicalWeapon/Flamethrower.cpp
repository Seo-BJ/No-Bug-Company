// Fill out your copyright notice in the Description page of Project Settings.


#include "07_Weapon/ConicalWeapon/Flamethrower.h"
#include "07_Weapon/WeaponSub/Wreckage.h"

#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h" 


AFlamethrower::AFlamethrower()
{
    WeaponID = FName(TEXT("Flamethrower"));

    bIsEvolved = false;
}

void AFlamethrower::BeginPlay()
{
    Super::BeginPlay();
    WeaponType = EWeaponType::Conical;
    DebugColor = FColor::Red;
}


void AFlamethrower::ApplyBurnDamage(AActor* Target)
{

    if (!IsValid(Target)) 
    {
        UE_LOG(LogTemp, Warning, TEXT("ApplyBurnDamage: Target is invalid or destroyed."));
        return;
    }

    // 데미지 적용
    UGameplayStatics::ApplyDamage(Target, BurnDamage, GetInstigatorController(), this, nullptr);
    UE_LOG(LogTemp, Log, TEXT("Burn Damage applied to %s: %f"), *Target->GetName(), BurnDamage);
}

void AFlamethrower::ApplyBurnEffect(APeCoEnemyCharacter* EnemyCharacter)
{
    if (!IsValid(EnemyCharacter))
    {
        UE_LOG(LogTemp, Warning, TEXT("ApplyBurnEffect: EnemyCharacter is invalid or already destroyed."));
        return;
    }

    if (!ActiveBurnTimers.Contains(EnemyCharacter))
    {
        FTimerHandle BurnTimerHandle;
        GetWorld()->GetTimerManager().SetTimer(BurnTimerHandle, FTimerDelegate::CreateLambda([=]()
            {
                ApplyBurnDamage(EnemyCharacter);

            }), BurnTickTime, true);

        ActiveBurnTimers.Add(EnemyCharacter, BurnTimerHandle);

        FTimerHandle ResetBurnHandle;
        GetWorld()->GetTimerManager().SetTimer(ResetBurnHandle, FTimerDelegate::CreateLambda([=]()
            {
                StopBurnEffect(EnemyCharacter);

            }), BurnDuration, false);

        UE_LOG(LogTemp, Log, TEXT("Burn effect applied to %s for %f seconds."),
            *EnemyCharacter->GetName(), BurnDuration);
    }
}

void AFlamethrower::StopBurnEffect(AActor* Target)
{
    if (!IsValid(Target))
    {
        UE_LOG(LogTemp, Warning, TEXT("StopBurnEffect: Target is invalid or already destroyed."));
        return;
    }

    if (ActiveBurnTimers.Contains(Target))
    {
        GetWorld()->GetTimerManager().ClearTimer(ActiveBurnTimers[Target]);
        ActiveBurnTimers.Remove(Target);
        UE_LOG(LogTemp, Log, TEXT("Burn effect stopped for %s"), *Target->GetName());
    }
}

void AFlamethrower::FlamethrowerEvolve()
{
    bIsEvolved = true;
}

void AFlamethrower::SpawnWreckage(FVector Location)
{
    FHitResult HitResult;
    FVector Start = Location + FVector(0.f, 0.f, 50.f);
    FVector End = Location - FVector(0.f, 0.f, 1000.f);

    FCollisionQueryParams TraceParams;
    TraceParams.AddIgnoredActor(this);
    TraceParams.bTraceComplex = true;

    if (GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECC_Visibility, TraceParams))
    {
        Location.Z = HitResult.Location.Z;
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("Failed to find ground for Wreckage spawn. Using default Z."));
    }

    if (AWreckage* Wreckage = GetWorld()->SpawnActor<AWreckage>(WreckageClass, Location, FRotator::ZeroRotator))
    {
        Wreckage->SetOwner(this);
        Wreckage->InitializeWreckage(WreckageDamage, WreckageTickTime, WreckageDuration);
        UE_LOG(LogTemp, Log, TEXT("Wreckage spawned at location: %s"), *Location.ToString());
    }
}

void AFlamethrower::Enhencement(int32 EnhencementIndex)
{
    if(EnhencementIndex == 0)
    {
        BurnDamage += 3;
    }
    if (EnhencementIndex == 1)
    {
        FireAngle += 5;
    }
}