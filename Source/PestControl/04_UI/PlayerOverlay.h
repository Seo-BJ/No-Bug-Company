// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "04_UI/PeCoUserWidget.h"
#include "PlayerOverlay.generated.h"


class PlayerOverlay;
class UGameTimerWidget;
class UPeCoProgressBar;
/**
 * 
 */
UCLASS()
class PESTCONTROL_API UPlayerOverlay : public UPeCoUserWidget
{
	GENERATED_BODY()

public:

	UPROPERTY(meta = (BindWidget))
	UGameTimerWidget* GameTimer;

	UPROPERTY(meta = (BindWidget))
	UPeCoProgressBar* HealthBar;

	UPROPERTY(meta = (BindWidget))
	UPeCoProgressBar* ExpBar;

};
