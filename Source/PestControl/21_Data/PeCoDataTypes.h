// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

UENUM(BlueprintType)
enum class ETeam : uint8
{
	ET_Player UMETA(DisplayName = "Player"),
	ET_Enemy UMETA(DisplayName = "Enemy"),
	ET_Neutral UMETA(DisplayName = "Neutral"),
};


UENUM(BlueprintType)
enum class EItemType : uint8
{
	EItemType_Consumable UMETA(DisplayName = "Consumable"),
	EItemType_Combat UMETA(DisplayName = "Combat"),

	EItemType_Default UMETA(DisplayName = "Default", Hidden),

};

UENUM(BlueprintType)
enum class EConsumableItemType : uint8
{
	EConsumableItemType_Default UMETA(DisplayName = "Default", Hidden),
};

enum class ECombatItemType : uint8
{
	ECombatItemType_Default UMETA(DisplayName = "Default", Hidden),
};


