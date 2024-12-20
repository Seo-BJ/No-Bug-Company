// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "PeCoGameplayTags.h"
#include "02_Player/PlayerStats.h"

#include "PeCoGameInstance.generated.h"

struct FPeCoStatData;
USTRUCT(BlueprintType)
struct FItemSaveData
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite)
	TSubclassOf<AActor> ItemClass; // 아이템 클래스

	UPROPERTY(BlueprintReadWrite)
	int32 Quantity; // 아이템 수량
};
/**
 * 
 */
UCLASS()
class PESTCONTROL_API UPeCoGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:

	UPeCoGameInstance();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Table", meta = (AllowPrivateAccess = "true"))
	FGameplayTag SelectedWeaponTag = PeCoGameplayTags::Weapon_Conical_Pesticide;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Table|UI", meta = (AllowPrivateAccess = "true"))
	TMap<FGameplayTag, TObjectPtr<UDataTable>> WidgetDataTableMap;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Table|Weapon", meta = (AllowPrivateAccess = "true"))
	TMap<FGameplayTag, UDataTable*> WeaponEnhancemenMaterialDataTableMap;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Table|Weapon", meta = (AllowPrivateAccess = "true"))
	TMap<FGameplayTag, UDataTable*> WeaponEvolveMaterialDataTableMap;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Table|Weapon", meta = (AllowPrivateAccess = "true"))
	UDataTable* WeaponInitInfoDataTable;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Table|Player", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UDataTable> StoreDataTable;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Table|Player", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UDataTable> SupplyDataTable;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Table|Player", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UDataTable> PlayerInitStatDataTable;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Table|Player", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UCurveTable> LevelUpCurveTable;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Table", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UCurveTable> StatUpgradeCurveTable;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Table|Enemy", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UDataTable> EnemyDropTable;

	int32 Money = 0;
	TArray<FItemSaveData> AllItems;

	
	UFUNCTION(BlueprintCallable)
	void SavePlayerInfo(AActor* Player);
	UFUNCTION(BlueprintCallable)
	void LoadPlayerInfo(AActor* Player);

	TMap<FGameplayTag, int32> PlayerStatMap;
	TMap<FGameplayTag, int32> WeaponStatMap;


	FPeCoStatData MaxHealth;
	FPeCoStatData MoveSpeed;
	FPeCoStatData DamageResistance;
	FPeCoStatData SkillCoolTime;

	float DamageMultiplier;
	float BaseDamage;
	float CooldownMultiplier;
	float CriticalChance;
	float CriticalDamageMultiplier;
	float Range;
};
