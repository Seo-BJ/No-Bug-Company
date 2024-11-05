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
	EItemType_Consumtion UMETA(DisplayName = "Consumable"),
	EItemType_Combat UMETA(DisplayName = "Combat"),

	EItemType_Default UMETA(DisplayName = "Default", Hidden),

};

UENUM(BlueprintType)
enum class EPlayerItem : uint8
{
	EPlayerItem_Health UMETA(DisplayName = "HealthItem"),
	EPlayerItem_Speed UMETA(DisplayName = "SpeedItem"),
	EPlayerItem_Power UMETA(DisplayName = "PowerItem"),
	EPlayerItem_Adrenaline UMETA(DisplayName = "AdrenalineItem"),
	EPlayerItem_Mine UMETA(DisplayName = "MineItem"),
	EPlayerItem_Adhesive UMETA(DisplayName = "AdhesiveItem"),
	EPlayerItem_Grenade UMETA(DisplayName = "GrenadeItem"),
	EPlayerItem_AntiSpray UMETA(DisplayName = "AntiSprayItem"),

	EPlayerItem_Default UMETA(DisplayName = "Default", Hidden),
};

UENUM(BlueprintType)
enum class ESlotType : uint8
{
	ESlotType_Weapon UMETA(DisplayName = "WeaponSlot"),
	ESlotType_Skill UMETA(DisplayName = "SkillSlot"),
	ESlotType_Consumption UMETA(DisplayName = "ConsumptionItemSlot"),
	ESlotType_Combat UMETA(DisplayName = "CombatItemSlot"),

	ESlotType_Default UMETA(DisplayName = "Default", Hidden),
};

UENUM(BlueprintType)
enum class EPlayerStatType : uint8
{
	EPlayerStatType_Health UMETA(DisplayName = "WeaponSlot"),
	EPlayerStatType_Speed UMETA(DisplayName = "SkillSlot"),
	ESlotType_Consumption UMETA(DisplayName = "ConsumptionItemSlot"),
	ESlotType_Combat UMETA(DisplayName = "CombatItemSlot"),

	ESlotType_Default UMETA(DisplayName = "Default", Hidden),
};



