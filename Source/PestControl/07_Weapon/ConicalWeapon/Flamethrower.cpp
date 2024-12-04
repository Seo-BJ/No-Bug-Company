// Fill out your copyright notice in the Description page of Project Settings.


#include "07_Weapon/ConicalWeapon/Flamethrower.h"
#include "07_Weapon/WeaponSub/Wreckage.h"

#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h" 


AFlamethrower::AFlamethrower()
{
    WeaponTag = PeCoGameplayTags::Weapon_Conical_Flamethrower;
    WeaponType = EWeaponType::Conical;
}

void AFlamethrower::BeginPlay()
{
    Super::BeginPlay();

    DebugColor = FColor::Red;
}


void AFlamethrower::ApplyBurnDamage(AActor* Target)
{
    if (!IsValid(Target))
    {
        UE_LOG(LogTemp, Warning, TEXT("ApplyBurnDamage: Target is invalid or already destroyed."));
        return;
    }

    UGameplayStatics::ApplyDamage(Target, BurnDamage, GetInstigatorController(), this, nullptr);
}

void AFlamethrower::ApplyBurnEffect(APeCoEnemyCharacter* EnemyCharacter)
{
    if (EvolveLevel <= 0) return;
    if (!IsValid(EnemyCharacter))
    {
        UE_LOG(LogTemp, Warning, TEXT("ApplyBurnEffect: EnemyCharacter is invalid or already destroyed."));
        return;
    }

    if (!ActiveBurnTimers.Contains(EnemyCharacter))
    {
        TWeakObjectPtr<APeCoEnemyCharacter> WeakEnemyCharacter = EnemyCharacter;

        FTimerHandle BurnTimerHandle;
        GetWorld()->GetTimerManager().SetTimer(BurnTimerHandle, FTimerDelegate::CreateLambda([=]()
            {
                if (WeakEnemyCharacter.IsValid())
                {
                    ApplyBurnDamage(WeakEnemyCharacter.Get());
                }
                else
                {
                    UE_LOG(LogTemp, Warning, TEXT("ApplyBurnEffect: WeakEnemyCharacter is no longer valid during timer callback."));
                }
            }), BurnTickTime, true);

        ActiveBurnTimers.Add(EnemyCharacter, BurnTimerHandle);

        FTimerHandle ResetBurnHandle;
        GetWorld()->GetTimerManager().SetTimer(ResetBurnHandle, FTimerDelegate::CreateLambda([=]()
            {
                StopBurnEffect(EnemyCharacter);
            }), BurnDuration, false);

        UE_LOG(LogTemp, Log, TEXT("Burn effect applied to %s for %f seconds."), *EnemyCharacter->GetName(), BurnDuration);
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
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("StopBurnEffect: Timer for Target not found."));
    }
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


bool AFlamethrower::EnhancementWeapon(int32 EnhancementIndex)
{
    if (Super::EnhancementWeapon(EnhancementIndex) == false) return false;
    FireAngle += 5;
    return true;
}

bool AFlamethrower::EvolveWeapon(int32 EvolveIndex)
{
    if (Super::EvolveWeapon(EvolveIndex) == false) return false;

    FlamethrowerEvolve();
    return true;
}
void AFlamethrower::FlamethrowerEvolve()
{

}