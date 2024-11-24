// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerStats.h"

FPeCoStatData::FPeCoStatData()
{
}


void FPeCoStatData::AddAdditiveBonus(float Amount)
{
	AdditiveBonus += Amount;
	UpdateCurrentValue();
}

void FPeCoStatData::AddMultiplierBonus(float Amount)
{
	MultiplierBonus += Amount;
	UpdateCurrentValue();
}

void FPeCoStatData::AddMultiplierReduction(float Amount)
{
	MultiplierReduction += Amount;
	UpdateCurrentValue();
}
