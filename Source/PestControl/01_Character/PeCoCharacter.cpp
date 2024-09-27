// Fill out your copyright notice in the Description page of Project Settings.


#include "PeCoCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystem.h"
#include "07_Weapon/Projectile.h"



// Sets default values
APeCoCharacter::APeCoCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	ProjectileSpawnPoint = CreateDefaultSubobject<USceneComponent>(TEXT("Spawn Point"));
	ProjectileSpawnPoint->SetupAttachment(RootComponent);

}

// Called when the game starts or when spawned
void APeCoCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void APeCoCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void APeCoCharacter::Fire()
{
	FVector Location = ProjectileSpawnPoint->GetComponentLocation();
	FRotator Rotation = ProjectileSpawnPoint->GetComponentRotation();

	AProjectile* Projectile = GetWorld()->SpawnActor<AProjectile>(ProjectileClass, Location, Rotation);
	Projectile->SetOwner(this);
}
