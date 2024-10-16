// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "21_Data/PeCoDataTypes.h"
#include "CombatInterface.generated.h"


class APeCoCharacter;

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

public:

	UFUNCTION()
	virtual void ReceiveDamage(AActor* DamagedActor, float Damage,const UDamageType* DamageType, AController* InstigatorController,AActor* DamageCauser);

	virtual void CharacterDie() = 0;

	//~Team 
	ETeam Team = ETeam::ET_Neutral;
	FORCEINLINE ETeam GetTeam() const { return Team; }
	void SetTeam(ETeam TeamToSet) { Team = TeamToSet; }

	//~End of Team

protected:

	//~Floating Text 

	// Show Floating Text when ReceiveDamage
	void ShowFloatingText(AActor* DamagedActor, AController* InstigatorController, float Damage, bool bCriticalHi = false, bool bBlockedHit = false) const;

	//~End of Floating Text


};
