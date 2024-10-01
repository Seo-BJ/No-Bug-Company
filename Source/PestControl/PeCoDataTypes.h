// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

UENUM(BlueprintType)
enum class ETeam : uint8
{
	ET_Player UMETA(DisplayName = "Player"),
	ET_Enemy UMETA(DisplayName = "Enemy"),
	ET_Neutral UMETA(DisplayName = "Neutral"),
};
