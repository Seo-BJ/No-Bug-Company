// Fill out your copyright notice in the Description page of Project Settings.


#include "00_GameModes/PeCoGameMode.h"

#include "01_Character/PeCoCharacter.h"
#include "01_Character/CombatInterface.h"

APeCoGameMode::APeCoGameMode()
{
}

void APeCoGameMode::BeginPlay()
{
	Super::BeginPlay();
	LevelStartingTime = GetWorld()->GetTimeSeconds();
}

void APeCoGameMode::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

float APeCoGameMode::CalculateDamage(AController* Attacker, AController* Victim, float BaseDamage)
{
	APawn* AttackerPawn = Attacker->GetPawn();
	APawn* VictimPawn = Victim->GetPawn();
	if (AttackerPawn && VictimPawn)
	{
		APeCoCharacter* AttackerCharacter = Cast<APeCoCharacter>(AttackerPawn);
		APeCoCharacter* VictimCharacter = Cast<APeCoCharacter>(VictimPawn);
		if (AttackerCharacter == VictimCharacter)
		{
			return BaseDamage;
		}
		if (AttackerCharacter->GetTeam() == VictimCharacter->GetTeam())
		{
			return 0.f;
		}
		
	}
	return BaseDamage;
}

