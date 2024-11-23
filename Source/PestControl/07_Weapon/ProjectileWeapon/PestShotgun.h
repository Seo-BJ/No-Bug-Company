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
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

};