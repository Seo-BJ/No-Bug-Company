// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "07_Weapon/Weapon.h"
#include "AirGun.generated.h"

/**
 * 
 */
UCLASS()
class PESTCONTROL_API AAirGun : public AWeapon
{
	GENERATED_BODY()
	
public:
	AAirGun();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon | Stun Effect", meta = (AllowPrivateAccess = "true"))
	float StunDuration = 2.0f; 
		
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon | Stun Effect", meta = (AllowPrivateAccess = "true"))
	bool bApplyStunEffect = true;

	void ApplyStunEffect(class APeCoEnemyCharacter* EnemyCharacter);
};