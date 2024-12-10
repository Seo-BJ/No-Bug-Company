// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include"01_Character/PeCoEnemyCharacter.h"
#include"07_Weapon/WeaponType.h"

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WeaponStats.h"
#include "PeCoGameplayTags.h"
#include "Weapon.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FFire, int32, CurrentAmmo, int32, MaxAamo);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FStartReload, float, Cooldown);

UCLASS()
class PESTCONTROL_API AWeapon : public AActor
{
	GENERATED_BODY()

public:

	AWeapon();

public:

	virtual void Tick(float DeltaTime) override;

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

	virtual void SpawnProjectile();

	void UpgradeWeapon(FGameplayTag StatTag, float UpgradeAmount);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TMap<FGameplayTag, int32> WeaponStatLevelMap; // 스텟별 업그레이드 단계 (업그레이드 가능한 스텟만 존재)

	UFUNCTION(BlueprintCallable)
	float GetStatValueByTag(FGameplayTag StatTag);

	UPROPERTY(BlueprintAssignable)
	FFire OnFire;

	UPROPERTY(BlueprintAssignable)
	FStartReload OnStartReload;

protected:

	virtual void BeginPlay() override;
	//~Weapon Stats

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FGameplayTag WeaponTag; // 각 무기별 GameplayTag
	UPROPERTY(EditAnywhere, Category = "Weapon Type")
	EWeaponType WeaponType; // 무기별 Type

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	float CriticalChance = 0.05f; // 업그레이드 가능
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	float CriticalDamageMultiplier = 1.5f;  // 업그레이드 가능

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	float BaseDamage = 10.0f;  // 업그레이드 가능
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	float DamageMultiplier = 1.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	float Delay = 0.f; // 발사체 간격
	UPROPERTY(EditAnywhere, Category = "Projectile Stats")
	int NumberOfProjectiles = 1; // 발사체 수

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	float Cooldown = 0.4f;  // 업그레이드 가능
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	float CooldownMultiplier = 1.f;

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
	float RangeRadius = 30; // 업그레이드 가능

	UPROPERTY(EditAnywhere, Category = "FanShaped Stats")
	float Range = 1300; // 업그레이드 가능
	//~End of Weapon Stats

	void FireWeapon();
	void DealDamageInSector();

	FColor DebugColor;

	//~ Weapon Upgrade

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	int32 EnhancementLevel = 0; // 강화 단계

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	int32 EvolveLevel = 0; // 진화 단계

	UFUNCTION(BlueprintCallable)
	bool CanEnhancementWeapon();
	UFUNCTION(BlueprintCallable)
	bool CanEvolveWeapon();

	UFUNCTION(BlueprintCallable)
	virtual bool EnhancementWeapon(int32 EnhancementIndex);
	UFUNCTION(BlueprintCallable)
	virtual bool EvolveWeapon(int32 EvolveIndex);

	UFUNCTION(BlueprintCallable)
	TMap<FGameplayTag, int32> GetWeaponMaterialData(int32 CurrentLevel, bool bEnhancement);

	//~End of Weapon Upgrade

private:



	void InitWeaponData();

	void StartReload();

	void Reload();

protected:
	bool bIsReloading = false;

	FTimerHandle FireTimerHandle;

	void ProjectileFire();

	void ShotgunFire();
	void StartShotgunCooldown();

	FTimerHandle CooldownHandle;
	virtual void ConicalFire();

public:

	bool HasWeaponEvolved() { return EvolveLevel > 0 ? true : false; }
	// 무기 스텟 관련 Get 함수들

	UFUNCTION(BlueprintCallable)
	float GetActualDamage() { return BaseDamage * DamageMultiplier; }
	UFUNCTION(BlueprintCallable)
	bool GetCriticalDamage(float& OutDamage);
	UFUNCTION(BlueprintCallable)
	float GetActualCoolDown() { return Cooldown / CooldownMultiplier; }
	UFUNCTION(BlueprintCallable)
	float GetAttackSpeedPerSecond() { return 1 / GetActualCoolDown(); }
	UFUNCTION(BlueprintCallable)
	int32 GetMaxAmmo() { return MaxAmmo; }
	UFUNCTION(BlueprintCallable)
	float GetReloadCoolDown() { return ReloadCoolDown; }
	UFUNCTION(BlueprintCallable)
	int GetNumberOfProjectiles() { return NumberOfProjectiles; }
	UFUNCTION(BlueprintCallable)
	float GetRange() { return Range; }
	void AddProjectile(int32 Amount) { NumberOfProjectiles += Amount; }
};
