// Fill out your copyright notice in the Description page of Project Settings.


#include "00_GameModes/StageThreeGameMode.h"


AStageThreeGameMode::AStageThreeGameMode()
{
	StageNumber = 1;
	StageTimeLimit = 600.f;

}

void AStageThreeGameMode::StageTimeEnd()
{
	Super::StageTimeEnd();


}
