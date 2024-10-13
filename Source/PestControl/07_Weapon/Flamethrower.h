// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "07_Weapon/Weapon.h"
#include "Flamethrower.generated.h"

/**
 * 
 */
UCLASS()
class PESTCONTROL_API AFlamethrower : public AWeapon
{
    GENERATED_BODY()
public:

    AFlamethrower();

protected:
    virtual void BeginPlay() override;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Effects")
    UParticleSystem* SprayEffect;

    FTimerHandle DurationTimeHandle;
    FTimerHandle CooldownHandle;

    float TickTime = 0.25f;

    void FlamethrowerFire();
    void DealDamage();
    void StartCooldown();

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Check Valid")
    bool bIsFlamethrowerValid = false;
};