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

DECLARE_MULTICAST_DELEGATE_TwoParams(FOnStatChanged, float, float);

USTRUCT(BlueprintType)
struct PESTCONTROL_API FPeCoStatData
{
	GENERATED_BODY()

	FPeCoStatData();
	FPeCoStatData(FGameplayTag StatTag)
		: StatTag(StatTag)
	{}

	FOnStatChanged OnStatChanged;

	void InitValue(float Value)
	{
		BaseValue = Value;
		CurrentValue = (BaseValue + AdditiveBonus) * (1 + MultiplierBonus) / (1 + MultiplierReduction);
	}
	void IncreaseLevel(int32 Amount)
	{
		StatLevel += Amount;
		NotifyStatChanged(StatLevel - Amount, StatLevel);
	}
	void SetBaseValue(float Value)
	{
		float OldBaseValue = BaseValue;
		BaseValue = Value;
		CurrentValue = (BaseValue + AdditiveBonus) * (1 + MultiplierBonus) / (1 + MultiplierReduction);
		NotifyStatChanged(OldBaseValue, BaseValue);
	}


	void AddAdditiveBonus(float Amount);
	void AddMultiplierBonus(float Amount);
	void AddMultiplierReduction(float Amount);
	

	float GetCurrentValue() const { return CurrentValue; }
	float GetBaseValue() const { return BaseValue; }
	FGameplayTag GetStatTag() const { return StatTag; }
	int32 GetStatLevel() const { return StatLevel; }
	float GetAdditiveBonus() const { return AdditiveBonus; }
	float GetMultiplierBonus() const { return MultiplierBonus; }
	float GetMultiplierReduction() const { return MultiplierReduction; }

protected:

	void UpdateCurrentValue()
	{
		float OldValue = CurrentValue;
		CurrentValue = (BaseValue + AdditiveBonus) * (1 + MultiplierBonus) / (1 + MultiplierReduction);
		NotifyStatChanged(OldValue, CurrentValue);
	}

	void NotifyStatChanged(float OldValue, float NewValue)
	{
		OnStatChanged.Broadcast(OldValue, NewValue);
	}

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Stat")
	FGameplayTag StatTag;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Stat")
	int32 StatLevel = 0;

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