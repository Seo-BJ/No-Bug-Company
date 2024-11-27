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
void AAntiSpray::BeginPlay()
{
	Super::BeginPlay();

}
void AAntiSpray::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}



void AAntiSpray::ActivateItem(AActor* User)
{
	if (IsValid(User))
	{
		AffectedUser = User;
		ActivateInvincibility(InvincibilityDuration, AffectedUser);
	}
}

void AAntiSpray::ActivateInvincibility(float Duration, AActor* User)
{
	if (IsValid(User))
	{
		User->SetCanBeDamaged(false);
		UE_LOG(LogTemp, Log, TEXT("Invincibility activated: CanBeDamaged = false"));
		GetWorldTimerManager().SetTimer(InvincibilityTimerHandle, this, &AAntiSpray::DeactivateInvincibility, Duration, false);
	}
}

void AAntiSpray::DeactivateInvincibility()
{
	AffectedUser->SetCanBeDamaged(true);
	UE_LOG(LogTemp, Log, TEXT("Invincibility deactivated: CanBeDamaged = true"));
}

