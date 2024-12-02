// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Wreckage.generated.h"

UCLASS()
class PESTCONTROL_API AWreckage : public AActor
{
    GENERATED_BODY()

public:
    AWreckage();

protected:
    virtual void BeginPlay() override;

public:
    virtual void Tick(float DeltaTime) override;

    void InitializeWreckage(float Damage, float TickInterval, float Duration);

private:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wreckage", meta = (AllowPrivateAccess = "true"))
    float WreckageDamage = 20.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wreckage", meta = (AllowPrivateAccess = "true"))
    float WreckageTickInterval = 0.5f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wreckage", meta = (AllowPrivateAccess = "true"))
    float WreckageDuration = 3.0f;

    FTimerHandle DamageTickHandle;
    FTimerHandle LifetimeHandle;

    AController* MyOwnerInstigator = nullptr;

    void ApplyDamage();

    void DestroyWerckage();

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
    USceneComponent* Root;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
    UStaticMeshComponent* Mesh;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Collision", meta = (AllowPrivateAccess = "true"))
    class USphereComponent* DamageArea;
};
