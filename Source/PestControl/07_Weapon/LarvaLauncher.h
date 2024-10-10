// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "07_Weapon/Weapon.h"
#include "LarvaLauncher.generated.h"

/**
 * 
 */
UCLASS()
class PESTCONTROL_API ALarvaLauncher : public AWeapon
{
	GENERATED_BODY()

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	FTimerHandle CooldownHandle;

	TArray<FTimerHandle> ProjectileTimerHandles;

public:
	ALarvaLauncher();
	
public:

	void LarvaLauncherFire();

	void SpawnProjectiles();

	UPROPERTY(EditAnywhere, Category = "Check Valid")
	bool bIsLarvaLauncherValid = true;
};
