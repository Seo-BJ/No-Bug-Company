// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "01_Character/PeCoCharacter.h"
#include "01_Character/CombatInterface.h"

#include "GameFramework/CharacterMovementComponent.h"
#include "10_Enemy/EnemyStats.h"

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

	// (EnemyID)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy")
	FName EnemyID;

	// ü��
	UPROPERTY(BlueprintReadWrite, Category = "Stats")
	float Health;

	// �ִ� ü��
	UPROPERTY(BlueprintReadWrite, Category = "Stats")
	float MaxHealth;

	// 적의 데미지 값
	UPROPERTY(BlueprintReadWrite, Category = "Stats")
	float Damage;

	// ������ ���̺����� ���� �� ����
	virtual void ApplyStatsFromData(const FEnemyStats& Stats);

private:

	AActor* VarDamageCauser;
	/*//~Enemy Stats
	UPROPERTY(EditAnywhere, Category = "EnemyStats")
	float Health;

	UPROPERTY(EditAnywhere, Category = "EnemyStats")
	float MaxHealth = 100;


	//~End of Enemy Stats*/

	//~Status effect
public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Status")
	bool bIsSlowed = false;  

	void ResetSlowStatus();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Status")
	bool bIsStun = false; 

	void ResetStunStatus();
	//~End of Status effect

	float GetMaxHealth();
};
