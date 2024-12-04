#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "PeCoGameplayTags.h"
#include "WeaponStats.generated.h"

USTRUCT(BlueprintType)
struct FWeaponStats : public FTableRowBase
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Stats")
    FGameplayTag WeaponTag;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Stats")
    float BaseDamage;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Stats")
    float DamageMultiplier;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Stats")
    float CriticalChance;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Stats")
    float CriticalDamageMultiplier;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Stats")
    float Cooldown;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Stats")
    float Delay;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Stats")
    int32 NumberOfProjectiles;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Stats")
    float FireAngle;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Stats")
    float RangeRadius;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Stats")
    int32 Ammo;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Stats")
    float ReloadCoolDown;
};