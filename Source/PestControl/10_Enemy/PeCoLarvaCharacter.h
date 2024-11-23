// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "10_Enemy/PeCoGroundEnemyCharacter.h"
#include "PeCoLarvaCharacter.generated.h"

/**
 * 
 */
UCLASS()
class PESTCONTROL_API APeCoLarvaCharacter : public APeCoGroundEnemyCharacter
{
	GENERATED_BODY()
	
public:
	APeCoLarvaCharacter();

protected:
	virtual void BeginPlay() override;

};
