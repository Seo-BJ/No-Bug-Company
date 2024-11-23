// Fill out your copyright notice in the Description page of Project Settings.


#include "09_Items/AntiSpray.h"\

#include "01_Character/PeCoPlayerCharacter.h"

// Sets default values
AAntiSpray::AAntiSpray()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	RootComponent = Root;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	Mesh->SetupAttachment(Root);

}

void AAntiSpray::ActivateItem(APeCoPlayerCharacter* Character)
{
	if (Character)
	{
		Character->ActivateInvincibility(InvincibilityDuration);
	}

	Destroy();
}

// Called when the game starts or when spawned
void AAntiSpray::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AAntiSpray::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

