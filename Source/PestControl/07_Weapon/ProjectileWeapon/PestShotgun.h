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

public:
	APestShotgun();

protected:

	virtual void BeginPlay() override;

	AWeapon* SecondShotgun;

private:

	virtual bool EnhancementWeapon(int32 EnhancementIndex) override;
	virtual bool EvolveWeapon(int32 EvolveIndex) override;
	void EvolvePestShotgun();

};