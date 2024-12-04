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
enum class ERewardRarity : uint8
{
	Common UMETA(DisplayName = "Common"),
	Rare UMETA(DisplayName = "Rare"),
	Epic UMETA(DisplayName = "Epic"),
	Legendary UMETA(DisplayName = "Legendary"),
};