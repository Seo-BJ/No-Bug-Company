// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "PeCoGameplayTags.h"

#include "PlayerStats.generated.h"

// Getter for CurrentValue
#define CHARACTERSTAT_VALUE_GETTER(StatName) \
	FORCEINLINE float Get##StatName() const \
	{ \
		return StatName.GetCurrentValue(); \
	}

// Setter for BaseValue
#define CHARACTERSTAT_BASEVALUE_SETTER(StatName) \
	FORCEINLINE void Set##Base##StatName(float NewValue) \
	{ \
		StatName.SetBaseValue(NewValue); \
	}

// Initializer for both BaseValue and CurrentValue
#define CHARACTERSTAT_VALUE_INITTER(StatName) \
	FORCEINLINE void Init##StatName(float NewValue) \
	{ \
		StatName.InitValue(NewValue); \
	}

// Combine all accessors
#define CHARACTERSTAT_ACCESSORS(StatName) \
	CHARACTERSTAT_VALUE_GETTER(StatName) \
	CHARACTERSTAT_BASEVALUE_SETTER(StatName) \
	CHARACTERSTAT_VALUE_INITTER(StatName)

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnStatChanged, float, Value);

USTRUCT(BlueprintType)
struct PESTCONTROL_API FPeCoStatData
{
	GENERATED_BODY()

	FPeCoStatData();
	FOnStatChanged OnStatChanged;

	FPeCoStatData(FGameplayTag StatTag)
		: StatTag(StatTag)
	{}

	float GetCurrentValue() const;

	float GetBaseValue() const
	{
		return BaseValue;
	}
	virtual void SetBaseValue(float NewValue)
	{
		BaseValue = NewValue; 
		OnStatChanged.Broadcast(GetCurrentValue());
	}
	virtual void InitValue(float NewValue)
	{
		BaseValue = NewValue;
		CurrentValue = NewValue;
	}

	int32 GetStatLevel() const
	{
		return StatLevel;
	}	
	void IncreaseLevel(int32 Amount)
	{
		StatLevel += Amount;
	}

	void AddAdditiveBonus(float Amount);
	void AddMultiplierBonus(float Amount);
	void AddMultiplierReduction(float Amount);
	

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Stat")
	FGameplayTag StatTag;

protected:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Stat")
	int32 StatLevel = 1;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Stat")
	float BaseValue = 0.f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Stat")
	float CurrentValue = 0.f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Stat")
	float AdditiveBonus = 0.f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Stat")
	float MultiplierBonus = 1.f;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Stat")
	float MultiplierReduction = 1.f;
};