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
/**
 * 
 */
UCLASS()
class PESTCONTROL_API APeCoPlayerCharacter : public APeCoCharacter
{
	GENERATED_BODY()

public:
	APeCoPlayerCharacter();

	// Called every frame.
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;

	/** Returns TopDownCameraComponent subobject **/
	FORCEINLINE class UCameraComponent* GetTopDownCameraComponent() const { return TopDownCameraComponent; }
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }

	virtual void PossessedBy(AController* NewController) override;
	// ~ End of Player Character Interface

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UInventoryComponent> InventoryComponent;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual ETeam GetTeam() override;

private:

	void InitPlayerCharacter();

	/** Top down camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* TopDownCameraComponent;

	/** Camera boom positioning the camera above the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;


	//Spawn Weapon
	//todo Ui클릭시 SpawnWeapon
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	USceneComponent* WeaponSpawnPoint;

	void SpawnLarvaLauncher();

	ALarvaLauncher* LarvaLauncherInstance;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	TSubclassOf<class AWeapon> LarvaLauncherClass;

	void SpawnWebRevolver();

	AWebRevolver* WebRevolverInstance;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	TSubclassOf<class AWeapon> WebRevolverClass;

	void SpawnPestShotgun();

	APestShotgun* PestShotgunInstance;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	TSubclassOf<class AWeapon> PestShotgunClass;

	void SpawnRoachShooter();

	ARoachShooter* RoachShooterInstance;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	TSubclassOf<class AWeapon> RoachShooterClass;

	void SpawnAirGun();

	AAirGun* AirGunInstance;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	TSubclassOf<class AWeapon> AirGunClass;

	void SpawnPesticide();

	APesticide* PesticideInstance;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	TSubclassOf<class AWeapon> PesticideClass;

	void SpawnFlamethrower();


	AFlamethrower* FlamethrowerInstance;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	TSubclassOf<class AWeapon> FlamethrowerClass;
	//End of Spawn Weapon

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
