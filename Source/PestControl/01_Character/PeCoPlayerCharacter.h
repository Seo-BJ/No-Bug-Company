// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "01_Character/PeCoCharacter.h"

#include "PeCoPlayerCharacter.generated.h"

class UInventoryComponent;
class UEquipmentComponent;
class UBuffComponent;
class AWeapon;

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


	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Weapons")
	TMap<FName, AWeapon*> SpawnedWeapons;

protected:

	virtual void BeginPlay() override;



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

	//~ Spray Bomb Test
	UFUNCTION(BlueprintCallable, Category = "SprayBomb")
	void SprayBombFire();

	UPROPERTY(EditDefaultsOnly, Category = "SprayBomb")
	TSubclassOf<class ASprayBomb> SprayBombClass;

	FVector GetCursorLocation();

	//~ End of Spray Bomb Test

protected:
	bool bIsInvincible;

	FTimerHandle InvincibilityTimerHandle;
	//~End of InvincibleState
};
