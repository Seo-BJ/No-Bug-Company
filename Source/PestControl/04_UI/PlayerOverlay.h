// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "04_UI/PeCoUserWidget.h"
#include "PeCoGameplayTags.h"
#include "21_Data/PeCoDataTypes.h"

#include "PlayerOverlay.generated.h"


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

	UFUNCTION(BlueprintImplementableEvent)
	void ShowInventoryWidget(FGameplayTag InventoryTypeTag);

	UFUNCTION(BlueprintImplementableEvent)
	void HideInventoryWidget(FGameplayTag InventoryTypeTag);

	//UFUNCTION(BlueprintImplementableEvent)
	UFUNCTION(BlueprintImplementableEvent)
	void ShowSupply(const TMap<FGameplayTag, int32>& SupplyResultMap);

	UFUNCTION(BlueprintImplementableEvent)
	void ShowDamageScrren();

};
	