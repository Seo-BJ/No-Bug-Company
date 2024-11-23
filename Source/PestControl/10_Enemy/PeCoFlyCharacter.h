// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "10_Enemy/PeCoFlyingEnemyCharacter.h"
#include "PeCoFlyCharacter.generated.h"

/**
 * 
 */
UCLASS()
class PESTCONTROL_API APeCoFlyCharacter : public APeCoFlyingEnemyCharacter
{
	GENERATED_BODY()
	
public:
	APeCoFlyCharacter();

protected:
	virtual void BeginPlay() override;

};
