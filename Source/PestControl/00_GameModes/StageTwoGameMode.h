// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "00_GameModes/PeCoGameMode.h"
#include "StageTwoGameMode.generated.h"

/**
 * 
 */
UCLASS()
class PESTCONTROL_API AStageTwoGameMode : public APeCoGameMode
{
	GENERATED_BODY()
public:
	AStageTwoGameMode();

protected:
	virtual void StageTimeEnd() override;
};
