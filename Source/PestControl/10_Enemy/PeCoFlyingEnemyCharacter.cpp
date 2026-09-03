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
    UCharacterMovementComponent* Movement = GetCharacterMovement();
    if (!ensureMsgf(Movement, TEXT("%s requires a CharacterMovement component"), *GetName()))
    {
        return;
    }
    Movement->MaxFlySpeed = FlyingSpeed;
    // DefaultLandMovementMode를 CDO에 직접 지정해둬야 풀 Acquire 시 CDO 기본값 복원 경로가
    // Flying으로 올바르게 돌아온다. (SetMovementMode는 런타임 호출이라 CDO 필드엔 반영되지 않음)
    Movement->DefaultLandMovementMode = EMovementMode::MOVE_Flying;
    Movement->SetMovementMode(EMovementMode::MOVE_Flying);

    // Remove Z-axis constraint
    Movement->bConstrainToPlane = false;
}

void APeCoFlyingEnemyCharacter::BeginPlay()
{
    Super::BeginPlay();   
          
    // Force enable flying mode in BeginPlay
    UCharacterMovementComponent* Movement = GetCharacterMovement();
    if (!ensureMsgf(Movement, TEXT("%s could not start: CharacterMovement is missing"), *GetName()))
    {
        return;
    }
    Movement->SetMovementMode(EMovementMode::MOVE_Flying);

    DefaultFlySpeed = FlyingSpeed;
                
}

void APeCoFlyingEnemyCharacter::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}

