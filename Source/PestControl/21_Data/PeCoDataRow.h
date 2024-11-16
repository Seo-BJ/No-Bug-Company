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
    int32 Level;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 RequiredKillCount;
};

USTRUCT(BlueprintType)
struct FPlayerStatData : public FTableRowBase
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 MaxHealth;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 MoveSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 AttackPower;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 AttackSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 DamageResistance;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 CriticalChance;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 CriticalDamage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Range;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 SkillCoolTime;
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

