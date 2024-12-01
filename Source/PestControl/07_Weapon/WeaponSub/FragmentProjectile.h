// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "07_Weapon/Projectile.h"
#include "FragmentProjectile.generated.h"

/**
 * 
 */
UCLASS()
class PESTCONTROL_API AFragmentProjectile : public AProjectile
{
    GENERATED_BODY()

public:
    AFragmentProjectile();

protected:
    virtual void BeginPlay() override;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Collision")
    class USphereComponent* CollisionComponent;

    UFUNCTION()
    void OnOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

    void StopMovement();

    void DestroyProjectile();

private:
    FTimerHandle FlightTimerHandle;
    FTimerHandle DestructionTimerHandle;

    bool bHasStopped; 
    float WebDuration = 2.0f; 
};