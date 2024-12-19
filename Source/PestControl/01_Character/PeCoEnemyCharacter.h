// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "01_Character/PeCoCharacter.h"
#include "01_Character/CombatInterface.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "10_Enemy/EnemyStats.h"
#include "PeCoGameplayTags.h"
#include "PeCoEnemyCharacter.generated.h"

struct FEnemyDropData;
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
	virtual void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

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

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy")
	FName EnemyID;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Enemy")
	FGameplayTag EnemyTag;

	UPROPERTY(BlueprintReadWrite, Category = "Stats")
	float Health;

	UPROPERTY(BlueprintReadWrite, Category = "Stats")
	float MaxHealth;

	// 적의 데미지 값
	UPROPERTY(BlueprintReadWrite, Category = "Stats")
	float Damage;

	virtual void ApplyStatsFromData(const FEnemyStats& Stats);

private:

	AActor* VarDamageCauser;

	void DropItem(bool bDropFlameSample);
	void AsyncLoadDropItem(FEnemyDropData* Row);
	void SpawnItem(UClass* ItemCalss);

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Status")
	bool bIsSlowed = false;  

	void ResetSlowStatus();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Status")
	bool bIsStun = false; 

	void ResetStunStatus();

	bool bIsWithered = false;

	bool bIsBurned = false;

	void ApplyTickDamage(float TickInterval, float DamagePerTick, float Duration, AActor* DamageCauser, AController* InstInstigator);
	//~End of Status effect

	float GetMaxHealth();

public:
	float DefaultFlySpeed = 400.0f; 
	float DefaultWalkSpeed = 300.0f;
};
