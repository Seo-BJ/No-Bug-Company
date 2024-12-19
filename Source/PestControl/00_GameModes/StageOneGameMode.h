// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "00_GameModes/PeCoGameMode.h"
#include "StageOneGameMode.generated.h"

/**
 * 
 */
UCLASS()
class PESTCONTROL_API AStageOneGameMode : public APeCoGameMode
{
	GENERATED_BODY()

public:
	AStageOneGameMode();

protected:
	virtual void StageTimeEnd() override;

	
};
