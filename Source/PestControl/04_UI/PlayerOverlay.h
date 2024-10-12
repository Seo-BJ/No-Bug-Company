// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "04_UI/PeCoUserWidget.h"
#include "PlayerOverlay.generated.h"


class PlayerOverlay;
class UGameTimerWidget;
class UPeCoProgressBar;
class UWeaponItemSlot;
class UPeCoUserWidget;
class USpecialEventWidget;
/**
 * 
 */
UCLASS()
class PESTCONTROL_API UPlayerOverlay : public UPeCoUserWidget
{
	GENERATED_BODY()

public:

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UGameTimerWidget* GameTimer;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UPeCoUserWidget* HealthBar;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UPeCoUserWidget* ExpBar;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UWeaponItemSlot* WeaponItemSlot;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	USpecialEventWidget* SpecialEvent;

};
