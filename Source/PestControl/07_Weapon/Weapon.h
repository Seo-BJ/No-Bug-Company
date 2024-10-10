// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Weapon.generated.h"

UCLASS()
class PESTCONTROL_API AWeapon : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AWeapon();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	//~Weapon Properties
	UPROPERTY(EditAnywhere, Category = "WeaponStats")
	float CriticalChance;
	UPROPERTY(EditAnywhere, Category = "WeaponStats")
	float CriticalDamageMultiplier;
	UPROPERTY(EditAnywhere, Category = "WeaponStats")
	float DamageMultiplier;
	UPROPERTY(EditAnywhere, Category = "WeaponStats")
	float BaseDamage;
	UPROPERTY(EditAnywhere, Category = "WeaponStats")
	float Cooldown;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Component", meta = (AllowPrivateAccess = "true"))
	USceneComponent* Root;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Component", meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* WeaponMesh;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Component", meta = (AllowPrivateAccess = "true"))
	USceneComponent* BulletSpawnPoint;

	//~End of Weapon propoerties

	//~ Projectile weapon properties
	UPROPERTY(EditAnywhere, Category = "Projectile Stats")
	int NumberOfProjectiles = 1;
	//~ end of projectile properties

	// ~ Conshaped weapon properties
	UPROPERTY(EditAnywhere, Category = "FanShaped Stats")
	float FireAngle;
	UPROPERTY(EditAnywhere, Category = "FanShaped Stats")
	float RangeRadius;
	UPROPERTY(EditAnywhere, Category = "FanShaped Stats")
	float DurationTime;
	// ~End of conshaped weapon properties
	void ProjectileFire();

	UPROPERTY(EditDefaultsOnly, Category = "Bullet")
	TSubclassOf<class AProjectile> BulletClass;
};
