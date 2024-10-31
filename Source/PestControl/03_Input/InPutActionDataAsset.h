// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "InPutActionDataAsset.generated.h"

class UInputAction;

UCLASS()
class PESTCONTROL_API UInPutActionDataAsset : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, Category = Input)
	UInputAction* InputActionMove;

	UPROPERTY(EditAnywhere, Category = Input)
	UInputAction* InputActionDash;

	UPROPERTY(EditAnywhere, Category = Input)
	UInputAction* InputActionSettings;

	UPROPERTY(EditAnywhere, Category = Input)
	UInputAction* InputActionChangeItem;

	UPROPERTY(EditAnywhere, Category = Input)
	UInputAction* InputActionUseConsumableItem;

	UPROPERTY(EditAnywhere, Category = Input)
	UInputAction* InputActionUseCombatItem;


	UPROPERTY(EditAnywhere, Category = Input)
	UInputAction* InputActionPressConsumableItem;

	UPROPERTY(EditAnywhere, Category = Input)
	UInputAction* InputActionPressCombatItem;


	
};
