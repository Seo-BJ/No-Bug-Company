// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "21_Data/PeCoDataTypes.h"
#include "CombatInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UCombatInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class PESTCONTROL_API ICombatInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:

	UFUNCTION()
	virtual void ReceiveDamage(AActor* DamagedActor, 
		float Damage,
		const UDamageType* DamageType,
		AController* InstigatorController,
		AActor* DamageCauser) = 0;

	virtual void CharacterDie() = 0;

	ETeam Team = ETeam::ET_Neutral;

	FORCEINLINE ETeam GetTeam() const { return Team; }
	void SetTeam(ETeam TeamToSet) { Team = TeamToSet; }




};
