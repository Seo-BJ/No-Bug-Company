// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "10_Enemy/PeCoGroundEnemyCharacter.h"
#include "PeCoRoachCharacter.generated.h"

/**
 * 
 */
UCLASS()
class PESTCONTROL_API APeCoRoachCharacter : public APeCoGroundEnemyCharacter
{
	GENERATED_BODY()
	
public:
	APeCoRoachCharacter();

protected:
	virtual void BeginPlay() override;


};
