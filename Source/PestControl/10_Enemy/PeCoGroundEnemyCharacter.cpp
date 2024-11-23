// Fill out your copyright notice in the Description page of Project Settings.


#include "10_Enemy/PeCoGroundEnemyCharacter.h"
#include "10_Enemy/PeCoGroundEnemyAIController.h"
#include "01_Character/PeCoPlayerCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "TimerManager.h"

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

    // Set default values for ground enemy properties
    PrimaryActorTick.bCanEverTick = true;

    
    
}

void APeCoGroundEnemyCharacter::BeginPlay()
{
    Super::BeginPlay();
      
}

void APeCoGroundEnemyCharacter::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
    Super::OnHit(HitComp, OtherActor, OtherComp, NormalImpulse, Hit); // 부모 클래스의 OnHit 호출
    if (OtherActor && OtherActor != this && OtherActor->IsA(APeCoPlayerCharacter::StaticClass()))
    {
        if (bRecentlyKnockedBack)
        {
            StopAIMovementDuringKnockback(1.0f); // 넉백 후 AI 이동 중단 시간을 설정
                        
        }
    }
 
}

void APeCoGroundEnemyCharacter::StopAIMovementDuringKnockback(float Duration)
{
    APeCoGroundEnemyAIController* AIController = Cast<APeCoGroundEnemyAIController>(GetController());
    if (AIController)
    {
        AIController->SetIsKnockedBack(true);

        GetWorld()->GetTimerManager().ClearTimer(KnockbackTimerHandle);
        // 타이머를 사용해 일정 시간 후  AI 이동을 원상복구
        GetWorld()->GetTimerManager().SetTimer(KnockbackTimerHandle, [this, AIController]() {
            AIController->SetIsKnockedBack(false);
            bRecentlyKnockedBack = false;
            }, Duration, false);
    }
}