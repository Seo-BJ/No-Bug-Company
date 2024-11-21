// Fill out your copyright notice in the Description page of Project Settings.


#include "PeCoCharacter.h"
#include "02_Player/PeCoPlayerState.h"


#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystem.h"
#include "07_Weapon/Projectile.h"



// Sets default values
APeCoCharacter::APeCoCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
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


ETeam APeCoCharacter::GetTeam()
{
	return ETeam::ET_Neutral;
}
