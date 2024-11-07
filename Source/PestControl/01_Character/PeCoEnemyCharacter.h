// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "01_Character/PeCoCharacter.h"
#include "01_Character/CombatInterface.h"

#include "GameFramework/Character.h"
#include "PeCoEnemyCharacter.generated.h"

/**
 * 
 */
UCLASS()
class PESTCONTROL_API APeCoEnemyCharacter : public APeCoCharacter, public ICombatInterface
{
	GENERATED_BODY()

public: 

	APeCoEnemyCharacter();

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	
	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	// Flag to prevent collision from being processed again for a certain time after the initial collision
	bool bRecentlyKnockedBack;

	// Function to reset the knockback flag after a certain amount of time
	void ResetKnockbackFlag();

	// Timer handle to reset the flag after a certain time has passed
	FTimerHandle KnockbackTimerHandle;
	
	
	UPROPERTY(EditAnywhere, Category = "Knockback")
	float KnockbackForce;


	//~ICombatInterface
	virtual void ReceiveDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType,
		AController* InstigatorController, AActor* DamageCauser) override;

	virtual void CharacterDie() override;
	//~End of ICombatInterface

private:
	//~Enemy Stats
	UPROPERTY(EditAnywhere, Category = "EnemyStats")
	float Health;

	UPROPERTY(EditAnywhere, Category = "EnemyStats")
	float MaxHealth = 100;


	//~End of Enemy Stats

	//~Status effect
public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Status")
	bool bIsSlowed = false;  

	void ResetSlowStatus();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Status")
	bool bIsStunned = false; 

	void ResetStunStatus();
	//~End of Status effect
};
