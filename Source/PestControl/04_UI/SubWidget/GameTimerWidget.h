// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "04_UI/PeCoUserWidget.h"
#include "GameTimerWidget.generated.h"

class UTextBlock;
/**
 * 
 */
UCLASS()
class PESTCONTROL_API UGameTimerWidget : public UPeCoUserWidget
{
	GENERATED_BODY()

public:

	UPROPERTY(meta = (BindWidget))
	UTextBlock* GameTimer;
	
};
