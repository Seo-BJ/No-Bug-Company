// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "07_Weapon/Projectile.h"
#include "AirGunEvolvedBullet.generated.h"

/**
 * 
 */
class UCapsuleComponent;

UCLASS()
class PESTCONTROL_API AAirGunEvolvedBullet : public AProjectile
{
    GENERATED_BODY()

public:
    AAirGunEvolvedBullet();

protected:
    virtual void BeginPlay() override;

private:
    bool bHasDealtDamage;

    UPROPERTY(VisibleAnywhere, Category = "Collision")
    UCapsuleComponent* OverlapCapsule;

    UFUNCTION()
    void OnOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
};