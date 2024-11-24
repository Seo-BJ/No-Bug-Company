// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include"01_Character/PeCoEnemyCharacter.h"
#include"07_Weapon/WeaponType.h"
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WeaponStats.h"
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

	//~ Weapon initialize
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon Data")
	UDataTable* WeaponDataTable;
	UPROPERTY(EditAnywhere, Category = "Weapon Type")
	EWeaponType WeaponType;

	FName WeaponID;

	int32 CurrentLevel;

	void LoadWeaponStats(int32 Level);

	UPROPERTY(EditAnywhere, Category = "Weapon Stats")
	float CriticalChance = 0.05f;
	UPROPERTY(EditAnywhere, Category = "Weapon Stats")
	float CriticalDamageMultiplier = 1.5f;
	UPROPERTY(EditAnywhere, Category = "Weapon Stats")
	float DamageMultiplier = 1.0f;
	UPROPERTY(EditAnywhere, Category = "Weapon Stats")
	float BaseDamage = 10.0f;
	UPROPERTY(EditAnywhere, Category = "Weapon Stats")
	float Cooldown = 0.4;
	UPROPERTY(EditAnywhere, Category = "Weapon Stats")
	float Delay = 0.f;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Component", meta = (AllowPrivateAccess = "true"))
	USceneComponent* Root;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Component", meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* WeaponMesh;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Component", meta = (AllowPrivateAccess = "true"))
	USceneComponent* BulletSpawnPoint;
	UPROPERTY(EditDefaultsOnly, Category = "Bullet")
	TSubclassOf<class AProjectile> BulletClass;
	UPROPERTY(EditAnywhere, Category = "Projectile Stats")
	int NumberOfProjectiles = 1;
	UPROPERTY(EditAnywhere, Category = "Weapon Stats")
	int32 Ammo = 1;
	UPROPERTY(EditAnywhere, Category = "Weapon Stats")
	int32 MaxAmmo = 1;
	UPROPERTY(EditAnywhere, Category = "Weapon Stats")
	float ReloadCoolDown = 1.0f;

	UPROPERTY(EditAnywhere, Category = "FanShaped Stats")
	float FireAngle = 30;
	UPROPERTY(EditAnywhere, Category = "FanShaped Stats")
	float RangeRadius = 30;

protected:
	void InitInfo();

	void DealDamageInSector();

	FVector InitialLocation;
	FRotator InitialRotation;

	FColor DebugColor;

public:
	UFUNCTION(BlueprintCallable, Category = "Weapon")
	void LevelUp();


private:

	void FireWeapon();

	void StartReload();

	void Reload();

	FTimerHandle CooldownHandle;
	FTimerHandle FireTimerHandle;

	void SpawnProjectile();

	void ProjectileFire();
	void StartProjectileCooldown();

	void ShotgunFire();
	void StartShotgunCooldown();

	void ConicalFire();
	void StartConicalCooldown();
};
