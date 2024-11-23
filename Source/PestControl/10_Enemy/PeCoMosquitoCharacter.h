// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "10_Enemy/PeCoFlyingEnemyCharacter.h"
#include "PeCoMosquitoCharacter.generated.h"

/**
 * 
 */
UCLASS()
class PESTCONTROL_API APeCoMosquitoCharacter : public APeCoFlyingEnemyCharacter
{
	GENERATED_BODY()

public:
	
	APeCoMosquitoCharacter();

protected:
	virtual void BeginPlay() override;
	
};
