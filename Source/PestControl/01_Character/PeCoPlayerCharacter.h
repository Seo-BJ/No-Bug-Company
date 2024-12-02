// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "01_Character/PeCoCharacter.h"
#include "PeCoGameplayTags.h"
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

	void RotateAim(FVector LookAtTarget);

	void MoveWeaponSpawnPoint(FVector MouseLocation);

	FORCEINLINE class UCameraComponent* GetTopDownCameraComponent() const { return TopDownCameraComponent; }

	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }

	virtual void PossessedBy(AController* NewController) override;
	// ~ End of Player Character Interface

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UInventoryComponent> InventoryComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UEquipmentComponent> EquipmentComponent;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual ETeam GetTeam() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Weapons")
	TMap<FGameplayTag, AWeapon*> SpawnedWeapons;


protected:

	virtual void BeginPlay() override;

private:

	void InitPlayerCharacter();

	FVector GetTargetCursorLocation();

	/** Top down camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* TopDownCameraComponent;

	/** Camera boom positioning the camera above the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;


	//~Spawn Weapon

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	USceneComponent* WeaponSpawnPoint;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon", meta = (AllowPrivateAccess = "true"))
	USceneComponent* SecondWeaponSpawnPoint;
public:
	FORCEINLINE USceneComponent* GetSecondWeaponSpawnPoint() { return SecondWeaponSpawnPoint; }

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	TSubclassOf<class AWeapon> LarvaLauncherClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	TSubclassOf<class AWeapon> WebRevolverClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	TSubclassOf<class AWeapon> PestShotgunClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	TSubclassOf<class AWeapon> RoachShooterClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	TSubclassOf<class AWeapon> AirGunClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	TSubclassOf<class AWeapon> PesticideClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	TSubclassOf<class AWeapon> FlamethrowerClass;

	UPROPERTY(EditDefaultsOnly, Category = "Weapons")
	TMap<FGameplayTag, TSubclassOf<AWeapon>> WeaponClassMap;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	AWeapon* PlayerWeapon;

private:
	void SpawnWeapon(FGameplayTag WeaponTag);

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

	void BecomeCrashInvincible(float Duration);

	void EndCrashInvincible();

	FORCEINLINE FVector GetWeaponSpawnPointLocation() { return WeaponSpawnPoint->GetComponentLocation(); }
protected:
	bool bIsCrashInvincible;

	FTimerHandle CrashInvincibilityTimerHandle;
	//~End of InvincibleStat
};
