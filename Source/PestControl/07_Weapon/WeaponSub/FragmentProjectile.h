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

public:
    // 풀링 재사용 시 타이머 재등록 및 상태 초기화.
    virtual void OnAcquired_Implementation(const FTransform& SpawnTransform, AActor* NewOwner, APawn* NewInstigator) override;

private:
    FTimerHandle FlightTimerHandle;
    FTimerHandle DestructionTimerHandle;

    bool bHasStopped;
    float WebDuration = 2.0f;

    /** Overlap 델리게이트 1회성 바인딩 가드 (부모의 Hit/Overlap 바인딩과 별개). */
    bool bFragmentOverlapBound = false;
};