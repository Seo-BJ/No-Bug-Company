// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "07_Weapon/Weapon.h"
#include "LarvaLauncher.generated.h"

/**
 * 
 */
UCLASS()
class PESTCONTROL_API ALarvaLauncher : public AWeapon
{
	GENERATED_BODY()

public:
	ALarvaLauncher();

	UFUNCTION(BlueprintCallable, Category = "Evolution")
	void FirstEvolveLarvaLauncher();

	UFUNCTION(BlueprintCallable, Category = "Evolution")
	void SecondEvolveLarvaLauncher();


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;


public:
	void ApplyWitherEffect(APeCoEnemyCharacter* EnemyCharacter);

	void ClearWitherEffect(APeCoEnemyCharacter* EnemyCharacter);

	virtual void SpawnProjectile() override;

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Wither Effect")
	float WitherDuration = 3.0f; 

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Wither Effect")
	float WitherDamage = 5.0f; 

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Wither Effect")
	float TickInterval = 1.0f;

	bool bIsFirstEvolved = false;

	bool bIsSecondEvolved = false;

	float SecondEvolveDamage = 20.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Component", meta = (AllowPrivateAccess = "true"))
	USceneComponent* EVBulletSpawnPoint;

private:
	TMap<AActor*, FTimerHandle> WitherTimers; 
	TMap<AActor*, FTimerHandle> EndWitherTimers;
};
