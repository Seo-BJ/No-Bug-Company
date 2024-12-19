// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "00_GameModes/PeCoGameMode.h"
#include "StageThreeGameMode.generated.h"

/**
 * 
 */
UCLASS()
class PESTCONTROL_API AStageThreeGameMode : public APeCoGameMode
{
	GENERATED_BODY()
public:
	AStageThreeGameMode();

protected:
	virtual void StageTimeEnd() override;
};
