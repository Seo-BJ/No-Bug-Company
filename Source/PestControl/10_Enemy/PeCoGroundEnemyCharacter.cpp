// Fill out your copyright notice in the Description page of Project Settings.


#include "10_Enemy/PeCoGroundEnemyCharacter.h"
#include "10_Enemy/PeCoGroundEnemyAIController.h"
#include "GameFramework/CharacterMovementComponent.h"

APeCoGroundEnemyCharacter::APeCoGroundEnemyCharacter()
{
    GetCharacterMovement()->MaxWalkSpeed = 300.f;//Set max move speed

    // Configure character movement
    bUseControllerRotationYaw = false;
    GetCharacterMovement()->bOrientRotationToMovement = true; // Rotate character to moving direction
    GetCharacterMovement()->RotationRate = FRotator(0.f, 720.f, 0.f);
    GetCharacterMovement()->bConstrainToPlane = true;
    GetCharacterMovement()->bSnapToPlaneAtStart = true;

    // Set the default AIController class
    AIControllerClass = APeCoGroundEnemyAIController::StaticClass();

    // Ensure the AIController automatically possesses the character
    AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
    
}

void APeCoGroundEnemyCharacter::BeginPlay()
{
    Super::BeginPlay();
      
}