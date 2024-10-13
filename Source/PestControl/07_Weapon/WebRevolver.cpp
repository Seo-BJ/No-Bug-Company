// Fill out your copyright notice in the Description page of Project Settings.


#include "07_Weapon/WebRevolver.h"

#include "01_Character/PeCoEnemyCharacter.h"

#include "Kismet/GameplayStatics.h"
#include "GameFramework/CharacterMovementComponent.h"

AWebRevolver::AWebRevolver()
{
    WeaponID = FName(TEXT("WebRevolver"));

    CriticalChance = 0.05f;
    CriticalDamageMultiplier = 1.5f;
    BaseDamage = 50.f;
    DamageMultiplier = 1.0f;
    Cooldown = 1.0f;
    NumberOfProjectiles = 1;
}

void AWebRevolver::BeginPlay()
{
    Super::BeginPlay();
    if (bIsWebRevolverValid)
    {
        GetWorld()->GetTimerManager().SetTimer(CooldownHandle, this, &AWebRevolver::WebRevolverFire, Cooldown, true);
    }
}

void AWebRevolver::WebRevolverFire()
{
    for (int32 i = 1; i < NumberOfProjectiles + BaseAmmo; i++)
    {
        FTimerHandle TempHandle;
        float Delay = FMath::Max(i * 0.2f, 0.2f);  

        GetWorld()->GetTimerManager().SetTimer(TempHandle, this, &AWebRevolver::SpawnProjectiles, Delay, false);
    }

    float TotalFireTime = FMath::Max((NumberOfProjectiles + BaseAmmo - 1 ) * 0.2f, 0.2f);
    GetWorld()->GetTimerManager().SetTimer(CooldownHandle, this, &AWebRevolver::StartCooldown, TotalFireTime, false);
}

void AWebRevolver::StartCooldown()
{
    GetWorld()->GetTimerManager().SetTimer(CooldownHandle, this, &AWebRevolver::WebRevolverFire, Cooldown, false);
}


void AWebRevolver::SpawnProjectiles()
{
    if (this)
    {
        AWeapon::ProjectileFire();
    }
}

void AWebRevolver::ApplySlowEffect(APeCoEnemyCharacter* EnemyCharacter)
{
    if (bApplySlowEffect && EnemyCharacter && !EnemyCharacter->bIsSlowed)  // 슬로우가 이미 적용 중인지 확인
    {
        UCharacterMovementComponent* MovementComponent = EnemyCharacter->GetCharacterMovement();
        if (MovementComponent)
        {
            float OriginalSpeed = MovementComponent->MaxWalkSpeed;
            MovementComponent->MaxWalkSpeed *= SlowMultiplier;
            EnemyCharacter->bIsSlowed = true;

            FTimerHandle ResetSpeedHandle;
            GetWorld()->GetTimerManager().SetTimer(ResetSpeedHandle, FTimerDelegate::CreateLambda([=]() {
                MovementComponent->MaxWalkSpeed = OriginalSpeed;
                EnemyCharacter->ResetSlowStatus();
                }), SlowDuration, false);

            UE_LOG(LogTemp, Log, TEXT("Applied slow effect to %s: Speed reduced to %f for %f seconds"),
                *EnemyCharacter->GetName(), MovementComponent->MaxWalkSpeed, SlowDuration);
        }
    }
}