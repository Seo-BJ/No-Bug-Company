// Fill out your copyright notice in the Description page of Project Settings.


#include "07_Weapon/AirGun.h"

#include "01_Character/PeCoEnemyCharacter.h"

#include "Kismet/GameplayStatics.h"

#include "GameFramework/CharacterMovementComponent.h"

AAirGun::AAirGun()
{
    WeaponID = FName(TEXT("AirGun"));

    CriticalChance = 0.05f;
    CriticalDamageMultiplier = 1.5f;
    BaseDamage = 85.f;
    DamageMultiplier = 1.0f;
    Cooldown = 0.5f;
    NumberOfProjectiles = 1;
}

void AAirGun::BeginPlay()
{
    Super::BeginPlay();
    if (bIsAirGunValid)
    {
        GetWorld()->GetTimerManager().SetTimer(CooldownHandle, this, &AAirGun::AirGunFire, Cooldown, true);
    }
}

void AAirGun::AirGunFire()
{
    for (int32 i = 1; i <= NumberOfProjectiles; i++)
    {
        FTimerHandle TempHandle;
        float Delay = FMath::Max(i * 0.1f, 0.1f);

        GetWorld()->GetTimerManager().SetTimer(TempHandle, this, &AAirGun::SpawnProjectiles, Delay, false);
    }

    float TotalFireTime = FMath::Max(NumberOfProjectiles * 0.1f, 0.1f);
    GetWorld()->GetTimerManager().SetTimer(CooldownHandle, this, &AAirGun::StartCooldown, TotalFireTime, false);
}

void AAirGun::StartCooldown()
{
    GetWorld()->GetTimerManager().SetTimer(CooldownHandle, this, &AAirGun::AirGunFire, Cooldown, false);
}

void AAirGun::SpawnProjectiles()
{
    if (this)
    {
        AWeapon::ProjectileFire();
    }
}

void AAirGun::ApplyStunEffect(APeCoEnemyCharacter* EnemyCharacter)
{
    if (bApplyStunEffect && EnemyCharacter && !EnemyCharacter->bIsStunned)  // 기절 중복 적용 방지
    {
        UCharacterMovementComponent* MovementComponent = EnemyCharacter->GetCharacterMovement();
        if (MovementComponent)
        {
            MovementComponent->StopMovementImmediately();  // 이동 멈추기
            MovementComponent->MaxWalkSpeed = 0.0f;  // 이동 속도를 0으로 설정
            EnemyCharacter->bIsStunned = true;  // 기절 상태로 설정

            // 기절 지속 시간 후 원래 속도로 복구 및 기절 상태 해제
            FTimerHandle ResetStunHandle;
            GetWorld()->GetTimerManager().SetTimer(ResetStunHandle, FTimerDelegate::CreateLambda([=]() {
                MovementComponent->MaxWalkSpeed = 600.f;  // 원래 속도로 복구 (예: 600)
                EnemyCharacter->ResetStunStatus();  // 기절 상태 해제
                }), StunDuration, false);

            UE_LOG(LogTemp, Log, TEXT("Applied stun effect to %s: Stunned for %f seconds"),
                *EnemyCharacter->GetName(), StunDuration);
        }
    }
}