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


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Component", meta = (AllowPrivateAccess = "true"))
	USceneComponent* EVBulletSpawnPoint;

	virtual void SpawnProjectile() override;

	UFUNCTION(BlueprintCallable, Category = "Evolution")
	void EvolveRoachShooter();

private:
	bool bIsEvolved = false;
};
