// Fill out your copyright notice in the Description page of Project Settings.


#include "07_Weapon/ProjectileWeapon/AirGun.h"
#include "07_Weapon/Projectile.h"

#include "01_Character/PeCoEnemyCharacter.h"

#include "Kismet/GameplayStatics.h"
#include "GameFramework/CharacterMovementComponent.h"

AAirGun::AAirGun()
{
    WeaponID = FName(TEXT("AirGun"));
}

void AAirGun::BeginPlay()
{
    Super::BeginPlay();
    WeaponType = EWeaponType::Projectile;
}

void AAirGun::ApplyStunEffect(APeCoEnemyCharacter* EnemyCharacter)
{
    if (bApplyStunEffect && EnemyCharacter && !EnemyCharacter->bIsStun)
    {
        UCharacterMovementComponent* MovementComponent = EnemyCharacter->GetCharacterMovement();
        if (MovementComponent)
        {
            bool bIsFlyingEnemy = MovementComponent->MovementMode == EMovementMode::MOVE_Flying;

            if (bIsFlyingEnemy)
            {
                MovementComponent->StopMovementImmediately();
                MovementComponent->MaxFlySpeed = 0.0f;
            }
            else
            {
                MovementComponent->StopMovementImmediately();
                MovementComponent->MaxWalkSpeed = 0.0f;
            }

            EnemyCharacter->bIsStun = true; 

            FTimerHandle ResetStunHandle;
            GetWorld()->GetTimerManager().SetTimer(ResetStunHandle, FTimerDelegate::CreateLambda([=]() {

                if (bIsFlyingEnemy)
                {
                    MovementComponent->MaxFlySpeed = 400.f; 
                }
                else
                {
                    MovementComponent->MaxWalkSpeed = 300.f; 
                }
                EnemyCharacter->ResetStunStatus(); 
                }), StunDuration, false);

            UE_LOG(LogTemp, Log, TEXT("Applied stun effect to %s: Stunned for %f seconds"),
                *EnemyCharacter->GetName(), StunDuration);
        }
    }
}

void AAirGun::EvolveAirGun()
{
    bIsEvolved = true;
}

void AAirGun::SpawnProjectile()
{
    if (!BulletSpawnPoint)
    {
        return;
    }

    TSubclassOf<AProjectile> ProjectileClass = bIsEvolved ? EvolvedBulletClass : BulletClass;
    if (!ProjectileClass)
    {
        UE_LOG(LogTemp, Warning, TEXT("ProjectileClass is null."));
        return;
    }

    FActorSpawnParameters SpawnParams;
    SpawnParams.Owner = this;
    SpawnParams.Instigator = Cast<APawn>(GetOwner());

    FVector Location = BulletSpawnPoint->GetComponentLocation();
    FRotator Rotation = BulletSpawnPoint->GetComponentRotation();

    AProjectile* Projectile = GetWorld()->SpawnActor<AProjectile>(ProjectileClass, Location, Rotation, SpawnParams);
    if (Projectile)
    {
        float ActualDamage = BaseDamage * DamageMultiplier;
        if (FMath::RandRange(0.f, 1.f) < CriticalChance)
        {
            ActualDamage *= CriticalDamageMultiplier;
        }

        Projectile->SetDamage(ActualDamage);
        Projectile->SetOwner(this);

        UE_LOG(LogTemp, Log, TEXT("Spawned Projectile: %s"), *Projectile->GetName());
    }
}

void AAirGun::ApplyKnockback(APeCoEnemyCharacter* Enemy, const FVector& HitLocation)
{
    if (!Enemy)
    {
        return;
    }

    // Calculate knockback direction
    FVector KnockbackDirection = Enemy->GetActorLocation() - HitLocation;
    KnockbackDirection.Normalize();

    // Apply consistent knockback force
    float ConsistentKnockbackForce = 1500.0f; // Set a uniform knockback force value
    FVector KnockbackVector = KnockbackDirection * ConsistentKnockbackForce;

    // Use LaunchCharacter for smooth knockback
    Enemy->LaunchCharacter(KnockbackVector, true, true);

    UE_LOG(LogTemp, Log, TEXT("Consistent Knockback applied to %s with force: %s"),
        *Enemy->GetName(), *KnockbackVector.ToString());
}

void AAirGun::Enhencement(int32 EnhencementIndex)
{
    StunDuration += 0.05f;
}
