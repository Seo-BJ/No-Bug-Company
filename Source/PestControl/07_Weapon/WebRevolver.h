// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "07_Weapon/Weapon.h"
#include "WebRevolver.generated.h"

/**
 * 
 */
UCLASS()
class PESTCONTROL_API AWebRevolver : public AWeapon
{
	GENERATED_BODY()

public:
	AWebRevolver();

private:
	FTimerHandle FireTimerHandle;

	int32 BaseAmmo = 6;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	FTimerHandle CooldownHandle;
	
	FTimerHandle StartCooldDown;

public:

	void WebRevolverFire();

	void StartCooldown();

	void SpawnProjectiles();

	
	UPROPERTY(EditAnywhere, Category = "Check Valid")
	bool bIsWebRevolverValid = false;
};
