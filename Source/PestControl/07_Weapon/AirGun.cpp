// Fill out your copyright notice in the Description page of Project Settings.


#include "07_Weapon/AirGun.h"

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
    if (bApplyStunEffect && EnemyCharacter && !EnemyCharacter->bIsStunned)
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

            EnemyCharacter->bIsStunned = true; 

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