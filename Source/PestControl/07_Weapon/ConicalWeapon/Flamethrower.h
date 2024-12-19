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
    void ApplyBurnEffect(APeCoEnemyCharacter* EnemyCharacter);

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Wreckage")
    TSubclassOf<class AWreckage> WreckageClass;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Effects", meta = (AllowPrivateAccess = "true"))
    UParticleSystemComponent* FlamethrowerParticle;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effects")
    FVector ParticleScale = FVector(1.0f, 1.0f, 1.0f);

    void SpawnWreckage(FVector Location);

protected:
    virtual void BeginPlay() override;

    virtual void Tick(float DeltaTime) override;

private:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Burn Effect", meta = (AllowPrivateAccess = "true"))
    float BurnDamage = 5.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Burn Effect", meta = (AllowPrivateAccess = "true"))
    float BurnDuration = 3.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Burn Effect", meta = (AllowPrivateAccess = "true"))
    float BurnTickInterval = 0.5f;

    TMap<AActor*, FTimerHandle> ActiveBurnTimers;

    void ApplyBurnDamage(AActor* Target);

    float WreckageDuration = 3.0f;
    float WreckageDamage = 20.0f;
    float WreckageTickTime = 0.5f;

    virtual bool EnhancementWeapon(int32 EnhancementIndex) override;
    virtual bool EvolveWeapon(int32 EvolveIndex) override;
    void FlamethrowerEvolve();
    virtual void ConicalFire() override;

    bool bHasPlayedStartSound = false;
public:
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Sound")
    class USoundBase* StartSound;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Sound")
    USoundBase* LoopSound;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Sound")
    USoundBase* EndSound;

    UPROPERTY()
    class UAudioComponent* LoopingAudioComponent;


//public:
    //~Particle Effect Parameter
    //UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flamethrower|Particle")
    //float MaxParticleLifetime;

    //UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flamethrower|Particle")
    //FVector MaxParticleInitialVelocity;

    //UFUNCTION(BlueprintCallable, Category = "Flamethrower|Particle")
    //void SetMaxParticleLifetime();

    //UFUNCTION(BlueprintCallable, Category = "Flamethrower|Particle")
    //void SetMaxParticleInitialVelocity();
};