// Fill out your copyright notice in the Description page of Project Settings.


#include "10_Enemy/PeCoMosquitoCharacter.h"

APeCoMosquitoCharacter::APeCoMosquitoCharacter()
{
    EnemyID = "Mosquito";
    EnemyTag = PeCoGameplayTags::Enemy_Mosquito_Normal;
}

void APeCoMosquitoCharacter::BeginPlay()
{
    Super::BeginPlay();
}