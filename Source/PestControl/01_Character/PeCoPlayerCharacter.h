// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "01_Character/PeCoCharacter.h"
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
	
	void SetNewFireRate(float NewFireRate);

	/** Top down camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* TopDownCameraComponent;

	/** Camera boom positioning the camera above the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	UPROPERTY(EditAnywhere, Category = "Combat")
	float FireRate;

	void FireWeapon();

	FTimerHandle TimerHandle;
	float TimerInterval;

	APlayerController* PeCoPlayerController;

	UPROPERTY(EditDefaultsOnly, Category = "Shotgun")
	TSubclassOf<class AProjectile> PestShotgunProjectileClass;

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

protected:
	bool bIsInvincible;

	FTimerHandle InvincibilityTimerHandle;
	//~End of InvincibleSta
};
