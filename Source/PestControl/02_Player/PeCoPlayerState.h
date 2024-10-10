// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "01_Character/CombatInterface.h"
#include "21_Data/PeCoDataTypes.h"
#include "PeCoPlayerState.generated.h"

//DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FPlayerExpChanged, APeCoPlayerState*, HealthComponent, float, OldValue, float, NewValue, AActor*, Instigator);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FPlayerHealthChanged, float, OldValue, float, NewValue, AActor*, Instigator);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FPlayerExpChanged, float, Exp);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FPlayerLevelChanged, float, Level);

class UCurveTable;
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

	void UpdateHUDHealth();


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

	UPROPERTY(BlueprintAssignable)
	FPlayerExpChanged OnExpChanged;

	UPROPERTY(BlueprintAssignable)
	FPlayerLevelChanged OnLevelChanged;

	UFUNCTION(BlueprintCallable)
	void AddToKillCount(int32 KillCountAmount);

	UFUNCTION(BlueprintCallable)
	int32 GetKillCount();

	UFUNCTION(BlueprintCallable)
	void SetKillCount(int32 KillCountAmount);

	UPROPERTY()
	uint32 Level = 1;



	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "LevelUp", meta = (AllowPrivateAccess = "true"))
	UCurveTable* LevelUpCurveTable; 
	// 레벨업을 처리하는 함수
	UFUNCTION(BlueprintCallable)
	void CheckLevelUp();


	// 레벨업 시 실행되는 함수
	void HandleLevelUp(int32 NewLevel);
	//~End of Player Stats

	uint32 GetCurrentLevelKillCount(uint32 CurrentLevel, uint32 CurrentKillCount);

private:

	UPROPERTY()
	int32 KillCount = 0;

	TObjectPtr<APeCoGameMode> PeCoGameMode = nullptr;
	TObjectPtr<APeCoPlayerController> PeCoPlayerController = nullptr;


public:

	UFUNCTION(BlueprintCallable)
	FORCEINLINE float GetMaxHealth() { return MaxHealth; }
	UFUNCTION(BlueprintCallable)
	FORCEINLINE float GetHealth() { return Health; }
	UFUNCTION(BlueprintCallable)
	int32 GetCurrentLevelRequiredKillCount();
};
