// Fill out your copyright notice in the Description page of Project Settings.


#include "01_Character/PeCoEnemyCharacter.h"

#include "00_GameModes/PeCoGameMode.h"
#include "01_Character/PeCoPlayerCharacter.h"
#include "02_Player/PeCoPlayerState.h"
#include "07_Weapon/ConicalWeapon/Flamethrower.h"
#include "Components/CapsuleComponent.h"

APeCoEnemyCharacter::APeCoEnemyCharacter()
{
	PrimaryActorTick.bCanEverTick = true;
		
	// Set KnockbackForce
	KnockbackForce = 1000.0f;

	// Set up Hit event
	GetCapsuleComponent()->OnComponentHit.AddDynamic(this, &APeCoEnemyCharacter::OnHit);

	// �⺻ EnemyID �� ���� (�ʿ�� �ڽ� Ŭ�������� �����)
	EnemyID = NAME_None;


	MaxHealth = 100.0f; // set max health
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

void APeCoEnemyCharacter::ReceiveDamage(AActor* DamagedActor, float InputDamage, const UDamageType* DamageType, AController* InstigatorController, AActor* DamageCauser)
{
	VarDamageCauser = DamageCauser;
	APeCoGameMode* PeCoGameMode = GetWorld()->GetAuthGameMode<APeCoGameMode>();
	check(PeCoGameMode);
	InputDamage = PeCoGameMode->CalculateDamage(InstigatorController, GetController(), InputDamage);
	
	float DamageToHealth = InputDamage;

	Health = FMath::Clamp(Health - DamageToHealth, 0.f, MaxHealth);

	if (Health <= 0.f)
	{
		//Flamethrower Wreckage
		if (DamageCauser && DamageCauser->IsA(AFlamethrower::StaticClass()))
		{
			AFlamethrower* Flamethrower = Cast<AFlamethrower>(DamageCauser);
			if (Flamethrower && Flamethrower->HasWeaponEvolved())
			{
				Flamethrower->SpawnWreckage(GetActorLocation());

			}
		}
		//End of Flamethrower Wreckage

		if (IsValid(InstigatorController))
		{
			APeCoPlayerState* PS = InstigatorController->GetPlayerState<APeCoPlayerState>();
			if (IsValid(PS))
			{
				PS->AddToKillCount(1);
			}
		}
		CharacterDie();
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
	bIsStun = false;
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

void APeCoEnemyCharacter::ApplyStatsFromData(const FEnemyStats& Stats)
{
	// ������ ���̺���� �� ����
	Health = Stats.Health;
	MaxHealth = Stats.Health;
	Damage = Stats.Damage; // ������ �� �߰� 
	

	// �̵� �ӵ� ����
	if (GetCharacterMovement()->MovementMode == EMovementMode::MOVE_Flying)
	{
		GetCharacterMovement()->MaxFlySpeed = Stats.FlySpeed;
	}
	else
	{
		GetCharacterMovement()->MaxWalkSpeed = Stats.WalkSpeed;
	}

	
}