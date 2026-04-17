// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "Engine/DataTable.h"
#include "UObject/NoExportTypes.h"
#include "21_Data/PeCoDataTypes.h"
#include "PeCoGameplayTags.h"

#include "PeCoDataRow.generated.h"

USTRUCT(BlueprintType)
struct FLevelUpData : public FTableRowBase
{
    GENERATED_BODY()

public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 Level = 0;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 RequiredKillCount = 0;
};

USTRUCT(BlueprintType)
struct FPlayerStatData : public FTableRowBase
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 MaxHealth = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 MoveSpeed = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 DamageResistance = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 SkillCoolTime = 0;
};

USTRUCT(BlueprintType)
struct FWidgetInfoData : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FGameplayTag GameplayTag;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UTexture2D* StatIconImage = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FText Name = FText();

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FText Description = FText();
};

USTRUCT(BlueprintType)
struct FStoreRewardPriceData : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FGameplayTag GameplayTag;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 CommonPrice = 0;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 RarePrice = 0;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 EpicPrice = 0;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 LegendaryPrice = 0;
};

USTRUCT(BlueprintType)
struct FWeaponEnhancementMaterialsData : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TMap<FGameplayTag, int32> RequiredMaterials; 
};

USTRUCT(BlueprintType)
struct FSupplyProbablity
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<AActor> SupplyActorClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FGameplayTag ItemTag;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<int32> Probabilities;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<int32> Amounts;
};
USTRUCT(BlueprintType)
struct FSupplyData : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 LastLevel = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FSupplyProbablity> SupplyMap;
};

USTRUCT(BlueprintType)
struct FEnemyDropData : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FGameplayTag ItemTag;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSoftClassPtr<AActor> Item;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float DropRate = 0.f;
};