// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerStats.h"

FPeCoStatData::FPeCoStatData()
{
}
float FPeCoStatData::GetCurrentValue() const
{
	return (BaseValue * (1 + MultiplierBonus) / (1 + MultiplierReduction)) + AdditiveBonus;
}


void FPeCoStatData::AddAdditiveBonus(float Amount)
{
	AdditiveBonus += Amount;
}

void FPeCoStatData::AddMultiplierBonus(float Amount)
{
	MultiplierBonus += Amount;
}

void FPeCoStatData::AddMultiplierReduction(float Amount)
{
	MultiplierReduction += Amount;
}
/*
float FPeCoStatData::GetFinalValue() const
{
	return ((BaseValue * (1 + MultiplierBonus) / (1 + MultiplierReduction)) + AdditiveBonus);
}

float FPeCoStatData::GetFinalValueInverse() const
{
	return ((BaseValue / (1 + MultiplierBonus) * (1 + MultiplierReduction)) + AdditiveBonus);
}
*/