// Fill out your copyright notice in the Description page of Project Settings.


#include "09_Items/FoodTrap.h"

#include "01_Character/PeCoEnemyCharacter.h"
#include "Components/SceneComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SphereComponent.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"

// Sets default values
AFoodTrap::AFoodTrap()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

    Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
    RootComponent = Root;

    Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
    Mesh->SetupAttachment(Root);

    DetectionRange = CreateDefaultSubobject<USphereComponent>(TEXT("DetectionRange"));
    DetectionRange->SetupAttachment(Mesh);
    DetectionRange->SetSphereRadius(200.f); 

    ExplodeRange = CreateDefaultSubobject<USphereComponent>(TEXT("ExplodeRange"));
    ExplodeRange->SetupAttachment(Mesh);
    ExplodeRange->SetSphereRadius(400.f);

    ExplodeDelay = 1.5f;
    
    bIsTrapActive = false;
}

// Called when the game starts or when spawned
void AFoodTrap::BeginPlay()
{
	Super::BeginPlay();

    DetectionRange->OnComponentBeginOverlap.AddDynamic(this, &AFoodTrap::OnEnemyOverlapped);

    GetWorld()->GetTimerManager().SetTimer(ExplodeTimerHandle, this, &AFoodTrap::ActivateTrap, SpawnDelay, false);
}

// Called every frame
void AFoodTrap::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AFoodTrap::OnEnemyOverlapped(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
    UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
    bool bFromSweep, const FHitResult& SweepResult)
{
    if(bIsTrapActive)
    { 
        APeCoEnemyCharacter* EnemyCharacter = Cast<APeCoEnemyCharacter>(OtherActor);
        if (EnemyCharacter) 
        {
           GetWorld()->GetTimerManager().SetTimer(ExplodeTimerHandle, this, &AFoodTrap::Explode, ExplodeDelay, false);

           UE_LOG(LogTemp, Log, TEXT("FoodTrap Activated!"));
        }
        
    }
}

void AFoodTrap::Explode()
{
    TArray<AActor*> ActorsInRange;
    ExplodeRange->GetOverlappingActors(ActorsInRange);

    for (AActor* Actor : ActorsInRange)
    {
        APeCoEnemyCharacter* EnemyCharacter = Cast<APeCoEnemyCharacter>(Actor);
        if (EnemyCharacter) 
        {
            UGameplayStatics::ApplyDamage(
                Actor,
                999.f,
                nullptr,
                this,
                UDamageType::StaticClass()
            );
        }
    }
    Destroy();
}

void AFoodTrap::ActivateTrap()
{
    if (!bIsTrapActive)
    {
        bIsTrapActive = true;      
    }
}