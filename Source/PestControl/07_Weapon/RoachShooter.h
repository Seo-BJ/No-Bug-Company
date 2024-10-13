// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "07_Weapon/Weapon.h"
#include "RoachShooter.generated.h"

/**
 * 
 */
UCLASS()
class PESTCONTROL_API ARoachShooter : public AWeapon
{
	GENERATED_BODY()

public:
	ARoachShooter();

private:
	FTimerHandle FireTimerHandle;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	FTimerHandle CooldownHandle;

public:

	void RoachShooterFire();

	void SpawnProjectiles();


	UPROPERTY(EditAnywhere, Category = "Check Valid")
	bool bIsRoachShooterValid = false;
	
};
