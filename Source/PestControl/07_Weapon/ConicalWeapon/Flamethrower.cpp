// Fill out your copyright notice in the Description page of Project Settings.


#include "07_Weapon/ConicalWeapon/Flamethrower.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h" 


AFlamethrower::AFlamethrower()
{
    WeaponID = FName(TEXT("Flamethrower"));
}

void AFlamethrower::BeginPlay()
{
    Super::BeginPlay();
    WeaponType = EWeaponType::Conical;
    DebugColor = FColor::Red;
}

void AFlamethrower::ApplyBurnEffect(APeCoEnemyCharacter* EnemyCharacter)
{
    UE_LOG(LogTemp, Log, TEXT("Applied burn effect Called"));
    if (EnemyCharacter && !ActiveBurnTimers.Contains(EnemyCharacter))  
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

        UE_LOG(LogTemp, Log, TEXT("Applied burn effect to %s: Burning for %f seconds"),
            *EnemyCharacter->GetName(), BurnDuration);
    }
}

void AFlamethrower::ApplyBurnDamage(AActor* Target)
{
    if (Target)
    {
        UGameplayStatics::ApplyDamage(Target, BurnDamage, GetInstigatorController(), this, nullptr);
        UE_LOG(LogTemp, Log, TEXT("Burn Damage applied to %s: %f"), *Target->GetName(), BurnDamage);
    }
}

void AFlamethrower::StopBurnEffect(AActor* Target)
{
    if (ActiveBurnTimers.Contains(Target))
    {
        GetWorld()->GetTimerManager().ClearTimer(ActiveBurnTimers[Target]);
        ActiveBurnTimers.Remove(Target);
        UE_LOG(LogTemp, Log, TEXT("Burn effect stopped for %s"), *Target->GetName());
    }
}