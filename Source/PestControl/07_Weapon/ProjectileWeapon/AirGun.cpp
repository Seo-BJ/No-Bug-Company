// Fill out your copyright notice in the Description page of Project Settings.


#include "07_Weapon/ProjectileWeapon/AirGun.h"
#include "07_Weapon/Projectile.h"

#include "01_Character/PeCoEnemyCharacter.h"

#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"

#include "Kismet/GameplayStatics.h"
#include "GameFramework/CharacterMovementComponent.h"

AAirGun::AAirGun()
{
    WeaponTag = PeCoGameplayTags::Weapon_Projectile_AirGun;
    WeaponType = EWeaponType::Projectile;
}

void AAirGun::BeginPlay()
{
    Super::BeginPlay();
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


void AAirGun::SpawnProjectile()
{
    if (!BulletSpawnPoint)
    {
        return;
    }

    TSubclassOf<AProjectile> ProjectileClass = HasWeaponEvolved() ? EvolvedBulletClass : BulletClass;
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
        float ActualDamage = 0.f;
        GetCriticalDamage(ActualDamage);

        Projectile->SetDamage(ActualDamage);
        Projectile->SetOwner(this);
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


bool AAirGun::EnhancementWeapon(int32 EnhancementIndex)
{
    if (Super::EnhancementWeapon(EnhancementIndex) == false) return false;
    StunDuration += 0.05f;
    return true;
}

bool AAirGun::EvolveWeapon(int32 EvolveIndex)
{
    if (Super::EvolveWeapon(EvolveIndex) == false) return false;
    EvolveAirGun();
    return true;
}
void AAirGun::EvolveAirGun()
{

}
