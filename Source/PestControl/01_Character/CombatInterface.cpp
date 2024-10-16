// Fill out your copyright notice in the Description page of Project Settings.


#include "01_Character/CombatInterface.h"

#include "01_Character/PeCoCharacter.h"

#include "02_Player/PeCoPlayerController.h"

// Add default functionality here for any ICombatInterface functions that are not pure virtual.

void ICombatInterface::ReceiveDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType, AController* InstigatorController, AActor* DamageCauser)
{
}

void ICombatInterface::ShowFloatingText(AActor* DamagedActor, AController* InstigatorController, float Damage, bool bCriticalHit, bool bBlockedHit) const
{
	if (DamagedActor && InstigatorController)
	{
		if (GetTeam() == ETeam::ET_Enemy)
		{
			// Enemy가 데미지를 받은 경우
			if (APeCoPlayerController* PC = Cast<APeCoPlayerController>(InstigatorController))
			{
				PC->ShowDamageText(Damage, Cast<APeCoCharacter>(DamagedActor), bCriticalHit, bBlockedHit);
			}
		}
		else if (GetTeam() == ETeam::ET_Player)
		{
			// 플레이어가 데미지를 받은 경우
			if (APeCoPlayerController* PC = Cast<APeCoPlayerController>(Cast<APeCoCharacter>(DamagedActor)->GetController()))
			{
				PC->ShowDamageText(Damage, Cast<APeCoCharacter>(DamagedActor), bCriticalHit, bBlockedHit);
			}
		}
	}
}
