// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include"01_Character/PeCoEnemyCharacter.h"
#include"07_Weapon/WeaponType.h"

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WeaponStats.h"
#include "PeCoGameplayTags.h"
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

	virtual void SpawnProjectile();

	//~ Weapon initialize
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon Data")
	UDataTable* WeaponDataTable;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Component", meta = (AllowPrivateAccess = "true"))
	USceneComponent* Root;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Component", meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* WeaponMesh;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Component", meta = (AllowPrivateAccess = "true"))
	USceneComponent* BulletSpawnPoint;
	UPROPERTY(EditDefaultsOnly, Category = "Bullet")
	TSubclassOf<class AProjectile> BulletClass;

private:
	void InitWeaponData();





public:

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FGameplayTag WeaponTag;

	UPROPERTY(EditAnywhere, Category = "Weapon Type")
	EWeaponType WeaponType;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	int32 EnhancementLevel = 0;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	int32 EvolveLevel = 0;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	float CriticalChance = 0.05f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	float CriticalDamageMultiplier = 1.5f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	float DamageMultiplier = 1.0f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	float BaseDamage = 10.0f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	float Cooldown = 0.4;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	float Delay = 0.f;

	UPROPERTY(EditAnywhere, Category = "Projectile Stats")
	int NumberOfProjectiles = 1;
	UPROPERTY(EditAnywhere, Category = "Weapon Stats")
	int32 Ammo = 1;
	UPROPERTY(EditAnywhere, Category = "Weapon Stats")
	int32 AmmoCost = 1;
	UPROPERTY(EditAnywhere, Category = "Weapon Stats")
	int32 MaxAmmo = 1;
	UPROPERTY(EditAnywhere, Category = "Weapon Stats")
	float ReloadCoolDown = 1.0f;
	UPROPERTY(EditAnywhere, Category = "FanShaped Stats")
	float FireAngle = 30;
	UPROPERTY(EditAnywhere, Category = "FanShaped Stats")
	float RangeRadius = 30;



	void FireWeapon();
	void DealDamageInSector();

	FColor DebugColor;

	UFUNCTION(BlueprintCallable)
	bool CanEnhancementWeapon();
	UFUNCTION(BlueprintCallable)
	bool CanEvolveWeapon();

	UFUNCTION(BlueprintCallable)
	virtual bool EnhancementWeapon(int32 EnhancementIndex);
	UFUNCTION(BlueprintCallable)
	virtual bool EvolveWeapon(int32 EvolveIndex);

private:

	void StartReload();

	void Reload();

	FTimerHandle CooldownHandle;
	FTimerHandle FireTimerHandle;

	void ProjectileFire();

	void ShotgunFire();
	void StartShotgunCooldown();

	void ConicalFire();


public:

	bool HasWeaponEvolved() { return EvolveLevel > 0 ? true : false; }

};
