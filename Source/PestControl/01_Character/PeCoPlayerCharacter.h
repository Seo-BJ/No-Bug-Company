// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "01_Character/PeCoCharacter.h"

#include "07_Weapon/LarvaLauncher.h"
#include "07_Weapon/WebRevolver.h"
#include "07_Weapon/PestShotgun.h"
#include "07_Weapon/RoachShooter.h"
#include "07_Weapon/AirGun.h"
#include "07_Weapon/Pesticide.h"
#include "07_Weapon/Flamethrower.h"

#include "PeCoPlayerCharacter.generated.h"

class UInventoryComponent;
class UEquipmentComponent;
class UBuffComponent;

/**
 * 
 */
UCLASS()
class PESTCONTROL_API APeCoPlayerCharacter : public APeCoCharacter
{
	GENERATED_BODY()

public:
	APeCoPlayerCharacter();

	virtual void Tick(float DeltaSeconds) override;
	virtual void PostInitializeComponents() override;

	/** Returns TopDownCameraComponent subobject **/
	FORCEINLINE class UCameraComponent* GetTopDownCameraComponent() const { return TopDownCameraComponent; }
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }

	virtual void PossessedBy(AController* NewController) override;
	// ~ End of Player Character Interface

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UInventoryComponent> InventoryComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UEquipmentComponent> EquipmentComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UBuffComponent> BuffComponent;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual ETeam GetTeam() override;

<<<<<<< HEAD
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Weapons")
	TMap<FName, AWeapon*> SpawnedWeapons;
=======
protected:

	virtual void BeginPlay() override;
>>>>>>> BeforeMergeBranch


private:

	void InitPlayerCharacter();

	/** Top down camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* TopDownCameraComponent;

	/** Camera boom positioning the camera above the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;


	//~Spawn Weapon

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	USceneComponent* WeaponSpawnPoint;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	TSubclassOf<class AWeapon> LarvaLauncherClass; 

	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	TSubclassOf<class AWeapon> WebRevolverClass;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	TSubclassOf<class AWeapon> PestShotgunClass;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	TSubclassOf<class AWeapon> RoachShooterClass;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	TSubclassOf<class AWeapon> AirGunClass;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	TSubclassOf<class AWeapon> PesticideClass;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	TSubclassOf<class AWeapon> FlamethrowerClass;

	UPROPERTY(EditDefaultsOnly, Category = "Weapons")
	TMap<FName, TSubclassOf<AWeapon>> WeaponClassMap;

	UPROPERTY(VisibleAnywhere, Category = "Weapons")
	TMap<FName, AWeapon*> WeaponInstanceMap;



	void SpawnWeapon(FName WeaponName);

	void InitializeWeaponClasses();

	//~End of Spawn Weapon

	APlayerController* PeCoPlayerController;

public:
	//~Player Crash damage
	UFUNCTION()
	void OnHit(UPrimitiveComponent* PlayerHitComponent, AActor* EnemyHitActor, UPrimitiveComponent* EnemyHitComp, FVector NormalImpulse, const FHitResult& Hit);

	//~ Become InvicibleState
	UPROPERTY(EditAnywhere, Category = "Crash Invincible")
	float CrashDamage = 10.0f;

	UPROPERTY(EditAnywhere, Category = "Crash Invincible")
	float CrashInvincibleDuration = 3.0f;
	//~End of Player Crash damage

	void BecomeInvincible(float InvincibleDuration);

	void EndInvincible();

	//for LevelUp Test... ToDo Delete after test
	UPROPERTY(BlueprintReadWrite, Category = "Weapon")
	AWeapon* EquippedPesticide;

	UPROPERTY(BlueprintReadWrite, Category = "Weapon")
	AWeapon* EquippedLaL;

	UPROPERTY(BlueprintReadWrite, Category = "Weapon")
	AWeapon* EquippedWR;

	UPROPERTY(BlueprintReadWrite, Category = "Weapon")
	AWeapon* EquippedPS;

	UPROPERTY(BlueprintReadWrite, Category = "Weapon")
	AWeapon* EquippedRS;

	UPROPERTY(BlueprintReadWrite, Category = "Weapon")
	AWeapon* EquippedAG;

	UPROPERTY(BlueprintReadWrite, Category = "Weapon")
	AWeapon* EquippedFT;
	// ~ End of for levelup test

protected:
	bool bIsInvincible;

	FTimerHandle InvincibilityTimerHandle;
	//~End of InvincibleState
};
