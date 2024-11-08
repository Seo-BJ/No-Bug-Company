// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "07_Weapon/Weapon.h"
#include "Pesticide.generated.h"

/**
 * 
 */
UCLASS()
class PESTCONTROL_API APesticide : public AWeapon
{
	GENERATED_BODY()
public:
    APesticide();

protected:
    virtual void BeginPlay() override;
};
