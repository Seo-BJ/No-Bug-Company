// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "04_UI/PeCoUserWidget.h"
#include "WeaponItemSlot.generated.h"

class UPeCoUserWidget;
/**
 * 
 */
UCLASS()
class PESTCONTROL_API UWeaponItemSlot : public UPeCoUserWidget
{
	GENERATED_BODY()


public:
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UPeCoUserWidget* WeaponSlot;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UPeCoUserWidget* MainItemSlot;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UPeCoUserWidget* SubItemSlot;

	
};
