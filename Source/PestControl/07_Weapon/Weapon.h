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

	void InitInfo();

	void ApplyDamageToEnemiesInRange();

	FVector InitialLocation;
	FRotator InitialRotation;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon Data")
	UDataTable* WeaponDataTable;

	FName WeaponID;

	int32 CurrentLevel;

	void LoadWeaponStats(int32 Level);

	UFUNCTION(BlueprintCallable, Category = "Weapon")
	void LevelUp();

	//~Weapon Properties
	UPROPERTY(EditAnywhere, Category = "Weapon Stats")
	float CriticalChance;
	UPROPERTY(EditAnywhere, Category = "Weapon Stats")
	float CriticalDamageMultiplier;
	UPROPERTY(EditAnywhere, Category = "Weapon Stats")
	float DamageMultiplier;
	UPROPERTY(EditAnywhere, Category = "Weapon Stats")
	float BaseDamage;
	UPROPERTY(EditAnywhere, Category = "Weapon Stats")
	float Cooldown;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Component", meta = (AllowPrivateAccess = "true"))
	USceneComponent* Root;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Component", meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* WeaponMesh;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Component", meta = (AllowPrivateAccess = "true"))
	USceneComponent* BulletSpawnPoint;
	UPROPERTY(EditDefaultsOnly, Category = "Bullet")
	TSubclassOf<class AProjectile> BulletClass;

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
	void ShotgunFire();	

	void ApplyBurnDamage(APeCoEnemyCharacter* EnemyCharacter);

	void ApplyDamageToEnemy(APeCoEnemyCharacter* Enemy, float Damage);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon | Burn Effect")
	float BurnDamage = 5.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon | Burn Effect")
	float BurnDuration = 3.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon | Burn Effect")
	float BurnTickTime = 0.5f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Type")
	EWeaponType WeaponType = EWeaponType::None;
};
