// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "01_Character/PeCoEnemyCharacter.h"
#include "PeCoGroundEnemyCharacter.generated.h"

/**
 * 
 */
UCLASS()
class PESTCONTROL_API APeCoGroundEnemyCharacter : public APeCoEnemyCharacter
{
	GENERATED_BODY()

public:
	
	APeCoGroundEnemyCharacter();

protected:
	
	virtual void BeginPlay() override;

};