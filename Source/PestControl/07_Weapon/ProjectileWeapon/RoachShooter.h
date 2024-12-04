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

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Component", meta = (AllowPrivateAccess = "true"))
	USceneComponent* EVBulletSpawnPoint;
	virtual void SpawnProjectile() override;

protected:

	virtual void BeginPlay() override;

private:

	virtual bool EnhancementWeapon(int32 EnhancementIndex) override;
	virtual bool EvolveWeapon(int32 EvolveIndex) override;
	void EvolveRoachShooter();

};
