// Fill out your copyright notice in the Description page of Project Settings.


#include "07_Weapon/WebRevolver.h"

#include "01_Character/PeCoEnemyCharacter.h"

#include "Kismet/GameplayStatics.h"
#include "GameFramework/CharacterMovementComponent.h"

AWebRevolver::AWebRevolver()
{
    WeaponID = FName(TEXT("WebRevolver"));
}

void AWebRevolver::BeginPlay()
{
    Super::BeginPlay();
    WeaponType = EWeaponType::Projectile;
}

void AWebRevolver::ApplySlowEffect(APeCoEnemyCharacter* EnemyCharacter)
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
                MovementComponent->MaxFlySpeed *= SlowMultiplier;
            }
            else
            {
                MovementComponent->MaxWalkSpeed *= SlowMultiplier;
            }

            EnemyCharacter->bIsSlowed = true;

            FTimerHandle ResetSpeedHandle;
            GetWorld()->GetTimerManager().SetTimer(ResetSpeedHandle, FTimerDelegate::CreateLambda([=]() {

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