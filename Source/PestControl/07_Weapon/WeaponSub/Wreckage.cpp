// Fill out your copyright notice in the Description page of Project Settings.


#include "07_Weapon/WeaponSub/Wreckage.h"

#include "01_Character/PeCoEnemyCharacter.h"
#include "Components/SphereComponent.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"

// Sets default values
AWreckage::AWreckage()
{
    PrimaryActorTick.bCanEverTick = true;

    Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
    RootComponent = Mesh;

    DamageArea = CreateDefaultSubobject<USphereComponent>(TEXT("DamageArea"));
    DamageArea->SetupAttachment(Mesh);

    DamageArea->SetSphereRadius(200.0f); // Adjust as needed
}

void AWreckage::BeginPlay()
{
    Super::BeginPlay();
}

void AWreckage::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}

void AWreckage::InitializeWreckage(float Damage, float TickInterval, float Duration)
{
    AActor* MyOwner = GetOwner();
    if (MyOwner == nullptr)
    {
        Destroy();
        return;
    }

    APawn* WeaponOwnerPawn = Cast<APawn>(MyOwner->GetOwner());
    if (WeaponOwnerPawn)
    {
        MyOwnerInstigator = WeaponOwnerPawn->GetController();
    }

    GetWorld()->GetTimerManager().SetTimer(DamageTickHandle, this, &AWreckage::ApplyDamage, TickInterval, true);

    GetWorld()->GetTimerManager().SetTimer(LifetimeHandle, this, &AWreckage::DestroyWerckage, Duration, false);
}

void AWreckage::ApplyDamage()
{
   
    TArray<AActor*> OverlappingActors;
    DamageArea->GetOverlappingActors(OverlappingActors);

    for (AActor* Actor : OverlappingActors)
    {
        if (Actor && Actor->IsA(APeCoEnemyCharacter::StaticClass()))
        {
            UGameplayStatics::ApplyDamage(Actor, WreckageDamage, MyOwnerInstigator, this, UDamageType::StaticClass());
        }
    }
}

void AWreckage::DestroyWerckage()
{
    Destroy();
}