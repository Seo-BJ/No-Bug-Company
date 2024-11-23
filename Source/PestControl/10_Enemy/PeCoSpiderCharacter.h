// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "10_Enemy/PeCoGroundEnemyCharacter.h"
#include "PeCoSpiderCharacter.generated.h"

/**
 * 
 */
UCLASS()
class PESTCONTROL_API APeCoSpiderCharacter : public APeCoGroundEnemyCharacter
{
	GENERATED_BODY()
	
public:
	APeCoSpiderCharacter();

protected:
	virtual void BeginPlay() override;

};
