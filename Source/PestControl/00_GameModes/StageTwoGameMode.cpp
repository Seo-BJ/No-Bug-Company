// Fill out your copyright notice in the Description page of Project Settings.


#include "00_GameModes/StageTwoGameMode.h"

AStageTwoGameMode::AStageTwoGameMode()
{
	StageNumber = 1;
	StageTimeLimit = 600.f;

}

void AStageTwoGameMode::StageTimeEnd()
{
	Super::StageTimeEnd();
	StageFinishAndStartNextStage();

}
