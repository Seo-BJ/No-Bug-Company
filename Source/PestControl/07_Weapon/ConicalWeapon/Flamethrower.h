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

public:
    void ApplyBurnEffect(APeCoEnemyCharacter* EnemyCharacter);

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Wreckage")
    TSubclassOf<class AWreckage> WreckageClass;

    bool bIsEvolved = false;

private:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Burn Effect", meta = (AllowPrivateAccess = "true"))
    float BurnDamage = 5.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Burn Effect", meta = (AllowPrivateAccess = "true"))
    float BurnDuration = 3.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Burn Effect", meta = (AllowPrivateAccess = "true"))
    float BurnTickTime = 0.5f;

    TMap<AActor*, FTimerHandle> ActiveBurnTimers;

    void ApplyBurnDamage(AActor* Target);
    void StopBurnEffect(AActor* Target);

public: 

    UFUNCTION(BlueprintCallable, Category = "Evolution")
    void FlamethrowerEvolve();

    void SpawnWreckage(FVector Location);

    float WreckageDuration = 3.0f;
    float WreckageDamage = 200.0f;
    float WreckageTickTime = 0.5f;

    virtual void Enhencement(int32 EnhencementIndex) override;

};