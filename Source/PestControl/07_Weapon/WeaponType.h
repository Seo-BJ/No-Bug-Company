// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"

/**
 * 
 */
UENUM(BlueprintType)
enum class EWeaponType : uint8
{
    Projectile UMETA(DisplayName = "Projectile"),
    Conical UMETA(DisplayName = "Conical"),
    Shotgun UMETA(DisplayName = "Shotgun")
};

