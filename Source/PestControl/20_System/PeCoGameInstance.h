// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "PeCoGameplayTags.h"

#include "PeCoGameInstance.generated.h"

/**
 * 
 */
UCLASS()
class PESTCONTROL_API UPeCoGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Table", meta = (AllowPrivateAccess = "true"))
	FGameplayTag SelectedWeaponTag = PeCoGameplayTags::Weapon_Conical_Pesticide;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Table", meta = (AllowPrivateAccess = "true"))
	TMap<FGameplayTag, TObjectPtr<UDataTable>> WidgetDataTableMap;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Table", meta = (AllowPrivateAccess = "true"))
	TMap<FGameplayTag, UDataTable*> WeaponEnhancemenMaterialDataTableMap;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Table", meta = (AllowPrivateAccess = "true"))
	TMap<FGameplayTag, UDataTable*> WeaponEvolveMaterialDataTableMap;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Table", meta = (AllowPrivateAccess = "true"))
	UDataTable* WeaponInitInfoDataTable;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Table", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UDataTable> StoreDataTable;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Table", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UDataTable> SupplyDataTable;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Table", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UDataTable> PlayerInitStatDataTable;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Table", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UCurveTable> StatUpgradeCurveTable;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Table", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UCurveTable> LevelUpCurveTable;

};
