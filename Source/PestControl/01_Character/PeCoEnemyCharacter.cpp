// Fill out your copyright notice in the Description page of Project Settings.


#include "01_Character/PeCoEnemyCharacter.h"

#include "00_GameModes/PeCoGameMode.h"
#include "01_Character/PeCoPlayerCharacter.h"
#include "02_Player/PeCoPlayerState.h"

#include "Components/CapsuleComponent.h"

APeCoEnemyCharacter::APeCoEnemyCharacter()
{
	PrimaryActorTick.bCanEverTick = true;
		
	// Set KnockbackForce
	KnockbackForce = 1000.0f;

	// Set up Hit event
	GetCapsuleComponent()->OnComponentHit.AddDynamic(this, &APeCoEnemyCharacter::OnHit);


	MaxHealth = 100.f; // set max health
	Health = MaxHealth; // when the game start, set health = max health
}

void APeCoEnemyCharacter::BeginPlay()
{
	Super::BeginPlay();
	OnTakeAnyDamage.AddDynamic(this, &APeCoEnemyCharacter::ReceiveDamage);
	SetTeam(ETeam::ET_Enemy);
	
}


void APeCoEnemyCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void APeCoEnemyCharacter::ReceiveDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType, AController* InstigatorController, AActor* DamageCauser)
{
	APeCoGameMode* PeCoGameMode = GetWorld()->GetAuthGameMode<APeCoGameMode>();
	check(PeCoGameMode);
	Damage = PeCoGameMode->CalculateDamage(InstigatorController, GetController(), Damage);
	
	float DamageToHealth = Damage;

	Health = FMath::Clamp(Health - DamageToHealth, 0.f, MaxHealth);

	// To Do
	// UpdateHUDHealth();
	// UpdateHUDShield();
	// PlayHitReactMontage();

	if (Health <= 0.f)
	{
		PeCoGameMode = PeCoGameMode == nullptr ? GetWorld()->GetAuthGameMode<APeCoGameMode>() : PeCoGameMode;
		if (PeCoGameMode)
		{
			CharacterDie();
		}
	}

}

void APeCoEnemyCharacter::CharacterDie()
{
	UE_LOG(LogTemp, Warning, TEXT("Enemy character has died."));
	
	APeCoGameMode* PeCoGameMode = GetWorld()->GetAuthGameMode<APeCoGameMode>();
	// To Do : PeCoGameMode -> EnemyEliminated 

	GetWorld()->GetTimerManager().ClearAllTimersForObject(this);

	Destroy();
}


void APeCoEnemyCharacter::ResetSlowStatus()
{
	bIsSlowed = false;
}

void APeCoEnemyCharacter::ResetStunStatus()
{
}

float APeCoEnemyCharacter::GetMaxHealth()
{
	return MaxHealth;
}

void APeCoEnemyCharacter::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if (OtherActor && OtherActor != this && OtherActor->IsA(APeCoPlayerCharacter::StaticClass()))
	{
		// Apply physical force when colliding with the player
		FVector KnockbackDirection = GetActorLocation() - OtherActor->GetActorLocation();
		KnockbackDirection.Normalize();

		//Use LaunchCharacter to smoothly knock back
		LaunchCharacter(KnockbackDirection * KnockbackForce, true, true);  // Keep Z-axis velocity

		bRecentlyKnockedBack = true;
				
		GetWorld()->GetTimerManager().SetTimer(KnockbackTimerHandle, this, &APeCoEnemyCharacter::ResetKnockbackFlag, 2.0f, false);  // Prevent re-collision for 2 seconds
			
	}
}

void APeCoEnemyCharacter::ResetKnockbackFlag()
{
	bRecentlyKnockedBack = false;
	 
}