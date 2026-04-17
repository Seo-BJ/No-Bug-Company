// Fill out your copyright notice in the Description page of Project Settings.


#include "07_Weapon/ProjectileWeapon/WebRevolver.h"
#include "07_Weapon/WeaponSub/FragmentProjectile.h"

#include "01_Character/PeCoEnemyCharacter.h"

#include "20_System/Pool/PeCoPoolSubsystem.h"

#include "Kismet/GameplayStatics.h"
#include "GameFramework/CharacterMovementComponent.h"

AWebRevolver::AWebRevolver()
{
    WeaponTag = PeCoGameplayTags::Weapon_Projectile_WebRevolver;
}

void AWebRevolver::BeginPlay()
{
    Super::BeginPlay();
    WeaponType = EWeaponType::Projectile;
}

void AWebRevolver::ApplySlowEffect(APeCoEnemyCharacter* EnemyCharacter, float SlowRate)
{
    if (bApplySlowEffect && EnemyCharacter && !EnemyCharacter->bIsSlowed)
    {
        UCharacterMovementComponent* MovementComponent = EnemyCharacter->GetCharacterMovement();
        if (MovementComponent)
        {
            bool bIsFlyingEnemy = MovementComponent->MovementMode == EMovementMode::MOVE_Flying;

            float OriginalSpeed = bIsFlyingEnemy ? MovementComponent->MaxFlySpeed : MovementComponent->MaxWalkSpeed;

            if (bIsFlyingEnemy)
            {
                MovementComponent->MaxFlySpeed *= 1.0f - SlowRate;
            }
            else
            {
                MovementComponent->MaxWalkSpeed *= 1.0f - SlowRate;
            }

            EnemyCharacter->bIsSlowed = true;

            FTimerHandle ResetSpeedHandle;
            GetWorld()->GetTimerManager().SetTimer(ResetSpeedHandle, FTimerDelegate::CreateLambda([=]()
                {
                    if (bIsFlyingEnemy)
                    {
                        MovementComponent->MaxFlySpeed = OriginalSpeed;
                    }
                    else
                    {
                        MovementComponent->MaxWalkSpeed = OriginalSpeed;
                    }
                    EnemyCharacter->ResetSlowStatus();
                }), SlowDuration, false);

            UE_LOG(LogTemp, Log, TEXT("Applied slow effect to %s: Speed reduced to %f for %f seconds"),
                *EnemyCharacter->GetName(), bIsFlyingEnemy ? MovementComponent->MaxFlySpeed : MovementComponent->MaxWalkSpeed, SlowDuration);
        }
    }
}



void AWebRevolver::SpawnFragmentProjectiles(const FVector& SpawnLocation, const FRotator& SpawnRotation)
{

    if (!FragmentProjectileClass) return;

    const int32 NumberOfFragments = 3;
    const float SplitAngle = 30.0f;
    float StartYaw = SpawnRotation.Yaw - (SplitAngle / 2.0f);

    UPeCoPoolSubsystem* Pool = GetWorld()->GetSubsystem<UPeCoPoolSubsystem>();
    APawn* InstigatorPawn = GetInstigator();

    for (int32 i = 0; i < NumberOfFragments; ++i)
    {
        FRotator NewRotation = SpawnRotation;
        NewRotation.Yaw = StartYaw + i * (SplitAngle / (NumberOfFragments - 1));

        AProjectile* FragmentProjectile = nullptr;
        if (Pool)
        {
            FragmentProjectile = Pool->Acquire<AProjectile>(FragmentProjectileClass, FTransform(NewRotation, SpawnLocation), this, InstigatorPawn);
        }
        else
        {
            FActorSpawnParameters SpawnParams;
            SpawnParams.Owner = this;
            SpawnParams.Instigator = InstigatorPawn;
            FragmentProjectile = GetWorld()->SpawnActor<AFragmentProjectile>(FragmentProjectileClass, SpawnLocation, NewRotation, SpawnParams);
        }

        if (FragmentProjectile)
        {
            FragmentProjectile->SetDamage(0);
        }
    }
}

void AWebRevolver::SpawnProjectile()
{
    Super::SpawnProjectile();
}

bool AWebRevolver::EnhancementWeapon(int32 EnhancementIndex)
{
    if (Super::EnhancementWeapon(EnhancementIndex) == false) return false;
    SlowDuration += 0.05f;
    SlowMultiplier += 0.05f;
    return false;
}

bool AWebRevolver::EvolveWeapon(int32 EvolveIndex)
{
    if (Super::EvolveWeapon(EvolveIndex) == false) return false;
    EvolveWebRevolver();
    return false;
}

void AWebRevolver::EvolveWebRevolver()
{
}
