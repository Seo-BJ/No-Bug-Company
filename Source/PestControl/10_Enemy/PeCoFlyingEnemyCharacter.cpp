// Fill out your copyright notice in the Description page of Project Settings.


#include "10_Enemy/PeCoFlyingEnemyCharacter.h"
#include "10_Enemy/PeCoFlyingEnemyAIController.h"
#include "AIController.h"
#include "GameFramework/CharacterMovementComponent.h"


APeCoFlyingEnemyCharacter::APeCoFlyingEnemyCharacter()
{
    PrimaryActorTick.bCanEverTick = true;

    // Set flying speed
    FlyingSpeed = 400.f;

    // Set the default AIController class
    AIControllerClass = APeCoFlyingEnemyAIController::StaticClass();

    // Ensure the AIController automatically possesses the character
    AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;

    // Set flying mode
    GetCharacterMovement()->MaxFlySpeed = FlyingSpeed; 
    GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Flying); 

    // Remove Z-axis constraint
    GetCharacterMovement()->bConstrainToPlane = false;  
}

void APeCoFlyingEnemyCharacter::BeginPlay()
{
    Super::BeginPlay();   
          
    // Force enable flying mode in BeginPlay
    GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Flying);

    DefaultFlySpeed = FlyingSpeed;
                
}

void APeCoFlyingEnemyCharacter::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
    // If flying mode is not active, force re-enable flying mode
    if (GetCharacterMovement()->MovementMode != EMovementMode::MOVE_Flying)
    {
        GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Flying);
    }
}



