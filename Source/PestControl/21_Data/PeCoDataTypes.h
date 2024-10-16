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
enum class EConsumableItemType : uint8
{
	EItemType_Potion UMETA(DisplayName = "Potion"),
	EItemType_Drink UMETA(DisplayName = "Drink"),
	EItemType_Trap UMETA(DisplayName = "Trap"),

	EItemType_Default UMETA(DisplayName = "Default", Hidden),

};

enum class ECombatItemType : uint8
{
	EItemType_Potion UMETA(DisplayName = "Potion"),
	EItemType_Drink UMETA(DisplayName = "Drink"),
	EItemType_Trap UMETA(DisplayName = "Trap"),

	EItemType_Default UMETA(DisplayName = "Default", Hidden),

};
