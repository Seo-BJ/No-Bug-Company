// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "07_Weapon/Weapon.h"
#include "PestShotgun.generated.h"

/**
 * 
 */
UCLASS()
class PESTCONTROL_API APestShotgun : public AWeapon
{
	GENERATED_BODY()

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	FTimerHandle CooldownHandle;

	TArray<FTimerHandle> ProjectileTimerHandles;

public:

	APestShotgun();

public:
	void PestShotgunFire();

	void SpawnProjectiles();

	UPROPERTY(EditAnywhere, Category = "Check Valid")
	bool bIsPestshogunValid = true;

	UPROPERTY(EditAnywhere, Category = "Weapon Stats")
	float SpreadAngle;

	FRotator SpreadRotation;
};