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

public:
	ALarvaLauncher();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
};
