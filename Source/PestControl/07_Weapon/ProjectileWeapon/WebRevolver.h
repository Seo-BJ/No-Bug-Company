// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "07_Weapon/Weapon.h"
#include "WebRevolver.generated.h"

/**
 * 
 */
UCLASS()
class PESTCONTROL_API AWebRevolver : public AWeapon
{
	GENERATED_BODY()

public:
	AWebRevolver();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:

	void ApplySlowEffect(APeCoEnemyCharacter* EnemyCharacter, float SlowRate);

	UPROPERTY(EditAnywhere, Category = "Check Valid")
	bool bIsWebRevolverValid = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon | Slow Effect", meta = (AllowPrivateAccess = "true"))
	float SlowMultiplier = 0.1f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon | Slow Effect", meta = (AllowPrivateAccess = "true"))
	float WebBallSlowMultiplier = 0.2f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon | Slow Effect", meta = (AllowPrivateAccess = "true"))
	float SlowDuration = 3.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon | Slow Effect", meta = (AllowPrivateAccess = "true"))
	bool bApplySlowEffect = true;

	UFUNCTION(BlueprintCallable, Category = "Evolution")
	void EvolveWebRevolver();

	void SpawnFragmentProjectiles(const FVector& SpawnLocation, const FRotator& SpawnRotation);

	virtual void SpawnProjectile() override;

	bool bIsEvolved = false;


	virtual void Enhencement(int32 EnhencementIndex) override;

private:

	UPROPERTY(EditDefaultsOnly, Category = "Projectile")
	TSubclassOf<AProjectile> FragmentProjectileClass;
};
