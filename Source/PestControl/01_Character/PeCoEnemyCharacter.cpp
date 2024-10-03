// Fill out your copyright notice in the Description page of Project Settings.


#include "01_Character/PeCoEnemyCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"

APeCoEnemyCharacter::APeCoEnemyCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	GetCharacterMovement()->MaxWalkSpeed = 300.f;//Set max move speed
		
	// Configure character movement
	bUseControllerRotationYaw = false;
	GetCharacterMovement()->bOrientRotationToMovement = true; // Rotate character to moving direction
	GetCharacterMovement()->RotationRate = FRotator(0.f, 720.f, 0.f);
	GetCharacterMovement()->bConstrainToPlane = true;
	GetCharacterMovement()->bSnapToPlaneAtStart = true;

	MaxHealth = 100.f; // set max health
	Health = MaxHealth; // when the game start, set health = max health
}

void APeCoEnemyCharacter::BeginPlay()
{
	Super::BeginPlay();
}

void APeCoEnemyCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

float APeCoEnemyCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	if (Health > 0)
	{
		Health -= DamageAmount;

		if (Health <= 0.f)
		{
			Health = 0.f;
			Die();
		}
	}

	return DamageAmount;
}

void APeCoEnemyCharacter::Die()
{
	UE_LOG(LogTemp, Warning, TEXT("Enemy Died"));
	
	Destroy();
}