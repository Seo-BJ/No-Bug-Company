// Fill out your copyright notice in the Description page of Project Settings.


#include "22_Level/LevelWeb.h"

#include "01_Character/PeCoPlayerCharacter.h"
#include "01_Character/PeCoEnemyCharacter.h"

#include "02_Player/PeCoPlayerState.h"

#include "Components/CapsuleComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

ALevelWeb::ALevelWeb()
{
    PrimaryActorTick.bCanEverTick = false;

    CapsuleComponent = CreateDefaultSubobject<UCapsuleComponent>(TEXT("CapsuleComponent"));
    RootComponent = CapsuleComponent;

    CapsuleComponent->SetCollisionResponseToChannel(ECC_GameTraceChannel1, ECR_Ignore); 
    CapsuleComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly); 
    CapsuleComponent->SetCollisionObjectType(ECC_WorldDynamic);

    MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
    MeshComponent->SetupAttachment(RootComponent);


    MeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);

    CapsuleComponent->OnComponentBeginOverlap.AddDynamic(this, &ALevelWeb::OnOverlapBegin);
    CapsuleComponent->OnComponentEndOverlap.AddDynamic(this, &ALevelWeb::OnOverlapEnd);
}

void ALevelWeb::BeginPlay()
{
    Super::BeginPlay();
}

void ALevelWeb::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    if (APeCoEnemyCharacter* EnemyCharacter = Cast<APeCoEnemyCharacter>(OtherActor))
    {
        ApplySlowEffectToEnemy(EnemyCharacter, EnemySlowPercentage);
    }
    else if (APeCoPlayerCharacter* PlayerCharacter = Cast<APeCoPlayerCharacter>(OtherActor))
    {
        ApplySlowEffectToPlayer(PlayerCharacter, PlayerSlowPercentage);
    }
}

void ALevelWeb::OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
    if (APeCoEnemyCharacter* EnemyCharacter = Cast<APeCoEnemyCharacter>(OtherActor))
    {
        ResetEnemySpeed(EnemyCharacter);
    }
    else if (APeCoPlayerCharacter* PlayerCharacter = Cast<APeCoPlayerCharacter>(OtherActor))
    {
        ResetPlayerSpeed(PlayerCharacter);
    }
}

void ALevelWeb::ApplySlowEffectToEnemy(APeCoEnemyCharacter* EnemyCharacter, float SlowRate)
{
    if (EnemyCharacter && !EnemyCharacter->bIsSlowed)
    {
        UCharacterMovementComponent* MovementComponent = EnemyCharacter->GetCharacterMovement();
        if (MovementComponent)
        {
            float& Speed = MovementComponent->MovementMode == EMovementMode::MOVE_Flying ? MovementComponent->MaxFlySpeed : MovementComponent->MaxWalkSpeed;

            Speed *= 1.0f - (SlowRate / 100.0f);
            EnemyCharacter->bIsSlowed = true;
        }
    }
}

void ALevelWeb::ResetEnemySpeed(APeCoEnemyCharacter* EnemyCharacter)
{
    if (EnemyCharacter && EnemyCharacter->bIsSlowed)
    {
        UCharacterMovementComponent* MovementComponent = EnemyCharacter->GetCharacterMovement();
        if (MovementComponent)
        {
            if (MovementComponent->MovementMode == EMovementMode::MOVE_Flying)
            {
                MovementComponent->MaxFlySpeed = EnemyCharacter->DefaultFlySpeed;
            }
            else
            {
                MovementComponent->MaxWalkSpeed = EnemyCharacter->DefaultWalkSpeed;
            }
            EnemyCharacter->ResetSlowStatus();
        }
    }
}

void ALevelWeb::ApplySlowEffectToPlayer(APeCoPlayerCharacter* PlayerCharacter, float SlowRate)
{
    if (PlayerCharacter)
    {
        UCharacterMovementComponent* MovementComponent = PlayerCharacter->GetCharacterMovement();
        if (MovementComponent)
        {
            MovementComponent->MaxWalkSpeed *= 1.0f - (SlowRate / 100.0f);
        }
    }
}

void ALevelWeb::ResetPlayerSpeed(APeCoPlayerCharacter* PlayerCharacter)
{
    if (PlayerCharacter)
    {
        UCharacterMovementComponent* MovementComponent = PlayerCharacter->GetCharacterMovement();
        if (MovementComponent)
        {
            APeCoPlayerState* PlayerState = Cast<APeCoPlayerState>(PlayerCharacter->GetPlayerState());
            if (PlayerState)
            {
                MovementComponent->MaxWalkSpeed = PlayerState->GetMoveSpeed();
            }
        }
    }
}