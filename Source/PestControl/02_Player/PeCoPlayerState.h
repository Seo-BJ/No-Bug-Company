// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "01_Character/CombatInterface.h"

#include "PeCoPlayerState.generated.h"

//DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FPlayerExpChanged, APeCoPlayerState*, HealthComponent, float, OldValue, float, NewValue, AActor*, Instigator);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FPlayerHealthChanged, float, OldValue, float, NewValue, AActor*, Instigator);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FPlayerExpChanged, float, OldValue, float, NewValue);

class UDataTable;
class APeCoGameMode;
class APeCoPlayerController;
/**
 * 
 */
UCLASS()
class PESTCONTROL_API APeCoPlayerState : public APlayerState, public ICombatInterface
{
	GENERATED_BODY()

public:

	APeCoPlayerState();

	//~ICombatInterface
	virtual void ReceiveDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType,
		AController* InstigatorController, AActor* DamageCauser) override;

	virtual void CharacterDie() override;
	//~End of ICombatInterface


	//~Player Stats
	UPROPERTY(EditAnywhere, Category = "PlayerStats")
	float Health;

	UPROPERTY(EditAnywhere, Category = "PlayerStats")
	float MaxHealth = 100;


		
	/*
	UPROPERTY(EditAnywhere, Category = "Player Stats")
	float Shield;

	UPROPERTY(EditAnywhere, Category = "Player Stats")
	float Maxshield = 100;
	*/

	UPROPERTY(BlueprintAssignable)
	FPlayerHealthChanged OnHealthChagned;

	UPROPERTY(BlueprintAssignable)
	FPlayerHealthChanged OnMaxHealthChanged;


	UFUNCTION(BlueprintCallable)
	void AddToKillCount(int32 KillCountAmount);

	UFUNCTION(BlueprintCallable)
	int32 GetKillCount();

	UFUNCTION(BlueprintCallable)
	void SetKillCount(int32 KillCountAmount);

	UPROPERTY()
	int32 CurrentLevel = 1;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "LevelUp", meta = (AllowPrivateAccess = "true"))
	UDataTable* LevelUpDataTable; 
	// 레벨업을 처리하는 함수
	UFUNCTION(BlueprintCallable)
	void CheckLevelUp();


	// 레벨업 시 실행되는 함수
	void HandleLevelUp(int32 NewLevel);
	//~End of Player Stats

private:

	UPROPERTY()
	int32 KillCount = 0;

	TObjectPtr<APeCoGameMode> PeCoGameMode = nullptr;
	TObjectPtr<APeCoPlayerController> PeCoPlayerController = nullptr;


public:

	UFUNCTION(BlueprintCallable)
	float GetMaxHealth() { return MaxHealth; }
	UFUNCTION(BlueprintCallable)
	float GetHealth() { return Health; }
};
