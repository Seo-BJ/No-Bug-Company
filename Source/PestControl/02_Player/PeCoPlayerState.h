// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "PlayerStats.h"
#include "01_Character/CombatInterface.h"
#include "21_Data/PeCoDataTypes.h"
#include "PeCoGameplayTags.h"

#include "PeCoPlayerState.generated.h"

//DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FPlayerExpChanged, APeCoPlayerState*, HealthComponent, float, OldValue, float, NewValue, AActor*, Instigator);
DECLARE_MULTICAST_DELEGATE_TwoParams(FPlayerStatChanged, float, float);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FPlayerExpChanged, float, Exp);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FPlayerLevelChanged, float, Level);

class UCurveTable;
class UDataTable;
class APeCoGameMode;
class APeCoPlayerController;
class UPlayerStatPresenterComponent;
class ULevelUpComponent;
/**
 * 
 */
UCLASS()
class PESTCONTROL_API APeCoPlayerState : public APlayerState, public ICombatInterface
{
	GENERATED_BODY()

public:

	APeCoPlayerState();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UPlayerStatPresenterComponent> PlayerStatPresenterComponent;

	void HandleHealthChagne(float Damage, AController* InstigatorController, AActor* DamageCauser);
	
	//~Character Stats

	void InitPlayerStat();

	UFUNCTION(BlueprintCallable)
	int32  GetStatUpgradeData(FGameplayTag StatTag);

	void UpgradeStat(FGameplayTag StatTag);

	UFUNCTION(BlueprintCallable)
	FGameplayTagContainer GetRandomStat(const int NumberOfRewards);

	void AddHealth(float Amount, AController* InstigatorController, AActor* DamageCauser);

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "CharacterStats")
	TArray<FPeCoStatData> CharacterStats;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "CharacterStats")
	FPeCoStatData Health  = FPeCoStatData(PeCoGameplayTags::PlayerStat_Health);
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "CharacterStats")
	FPeCoStatData MaxHealth = FPeCoStatData(PeCoGameplayTags::PlayerStat_MaxHealth);
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "CharacterStats")
	FPeCoStatData MoveSpeed = FPeCoStatData(PeCoGameplayTags::PlayerStat_MoveSpeed);
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "CharacterStats")
	FPeCoStatData AttackPower = FPeCoStatData(PeCoGameplayTags::PlayerStat_AttackPower);
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "CharacterStats")
	FPeCoStatData AttackSpeed = FPeCoStatData(PeCoGameplayTags::PlayerStat_AttackSpeed);
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "CharacterStats")
	FPeCoStatData DamageResistance = FPeCoStatData(PeCoGameplayTags::PlayerStat_DamageResistance);
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "CharacterStats")
	FPeCoStatData CriticalChance = FPeCoStatData(PeCoGameplayTags::PlayerStat_CriticalChance);
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "CharacterStats")
	FPeCoStatData CriticalDamage = FPeCoStatData(PeCoGameplayTags::PlayerStat_CriticalDamage);
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "CharacterStats")
	FPeCoStatData Range = FPeCoStatData(PeCoGameplayTags::PlayerStat_Range);
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "CharacterStats")
	FPeCoStatData SkillCoolTime = FPeCoStatData(PeCoGameplayTags::PlayerStat_SkillCoolTime);


	UFUNCTION(BlueprintCallable)
	FPeCoStatData GetStatByTag(FGameplayTag Tag);

	CHARACTERSTAT_ACCESSORS(Health);
	CHARACTERSTAT_ACCESSORS(MaxHealth);
	CHARACTERSTAT_ACCESSORS(MoveSpeed);
	CHARACTERSTAT_ACCESSORS(AttackPower);
	CHARACTERSTAT_ACCESSORS(AttackSpeed);
	CHARACTERSTAT_ACCESSORS(DamageResistance);
	CHARACTERSTAT_ACCESSORS(CriticalChance);
	CHARACTERSTAT_ACCESSORS(CriticalDamage);
	CHARACTERSTAT_ACCESSORS(Range);
	CHARACTERSTAT_ACCESSORS(SkillCoolTime);

	//~End of Character Stats
	
	//~ICombatInterface
	virtual void ReceiveDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType, AController* InstigatorController, AActor* DamageCauser) override;
	virtual void CharacterDie() override;
	//~End of ICombatInterface



	//~Player Level, Exp System

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


	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Table", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UDataTable> WidgetDataTable;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Table", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UDataTable> PlayerInitStatDataTable;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Table", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UCurveTable> StatUpgradeCurveTable;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Table", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UCurveTable> LevelUpCurveTable;

	// 레벨업을 처리하는 함수
	UFUNCTION(BlueprintCallable)
	void CheckLevelUp();

	// 레벨업 시 실행되는 함수
	UFUNCTION(BlueprintCallable)
	void HandleLevelUp(int32 NewLevel);



	//~End of Player Stats

	uint32 GetCurrentLevelKillCount(uint32 CurrentLevel, uint32 CurrentKillCount);

	//~End of Player Level, Exp System

protected:

	void BeginPlay() override;

	template<typename T>
	T* GetDataTableRowByTag(UDataTable* DataTable, const FGameplayTag& Tag);

private:

	int Level = 1;

	int32 KillCount = 0;

public:

	UFUNCTION(BlueprintCallable, BlueprintPure)
	FORCEINLINE int GetPlayerLevel() { return Level; }
	UFUNCTION(BlueprintCallable, BlueprintPure)
	int32 GetCurrentLevelRequiredKillCount();




};

template <typename T>
T* APeCoPlayerState::GetDataTableRowByTag(UDataTable* DataTable, const FGameplayTag& Tag)
{
	return DataTable->FindRow<T>(Tag.GetTagName(), TEXT(""));
}