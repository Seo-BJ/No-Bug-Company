// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "07_Weapon/Weapon.h"
#include "AirGun.generated.h"

/**
 * 
 */
UCLASS()
class PESTCONTROL_API AAirGun : public AWeapon
{
	GENERATED_BODY()
	
public:

	AAirGun();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon | Stun Effect", meta = (AllowPrivateAccess = "true"))
	float StunDuration = 0.1f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon | Stun Effect", meta = (AllowPrivateAccess = "true"))
	bool bApplyStunEffect = true;

	void ApplyStunEffect(class APeCoEnemyCharacter* EnemyCharacter);

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon|Knockback")
	float KnockbackForce = 1500.0f;

	void ApplyKnockback(APeCoEnemyCharacter* Enemy, const FVector& HitLocation);

protected:

	virtual void BeginPlay() override;

	virtual void SpawnProjectile() override;

private:
	UPROPERTY(EditDefaultsOnly, Category = "Weapon | Evolution")
	TSubclassOf<AProjectile> EvolvedBulletClass;

	virtual bool EnhancementWeapon(int32 EnhancementIndex) override;
	virtual bool EvolveWeapon(int32 EvolveIndex) override;
	void EvolveAirGun();
};