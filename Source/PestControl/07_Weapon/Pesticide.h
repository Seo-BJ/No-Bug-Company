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
    // 기본 생성자
    APesticide();

protected:
    virtual void BeginPlay() override;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Effects")
    UParticleSystem* SprayEffect;

    FTimerHandle DurationTimeHandle;
    FTimerHandle CooldownHandle;

    float TickTime = 0.25f;

    void PesticideFire();
    void DealDamage();
    void StartCooldown();

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Check Valid")
    bool bIsPesticideValid = true;
};
