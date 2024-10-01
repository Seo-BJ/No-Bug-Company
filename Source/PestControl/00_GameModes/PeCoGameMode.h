// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "PeCoGameMode.generated.h"

/**
 * 
 */
UCLASS()
class PESTCONTROL_API APeCoGameMode : public AGameMode
{
	GENERATED_BODY()

public:

	APeCoGameMode();

	//~AGameMode Interface

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	//~End of AGameMode Interface


	float CalculateDamage(AController* Attacker, AController* Victim, float BaseDamage);

#pragma region Game Time & Timer

	// 1200.f = 1200УЪ = 20Ка
	UPROPERTY(EditDefaultsOnly)
	float TotalGameTime = 1200.f; 

	float LevelStartingTime = 0.f;

#pragma endregion




};
