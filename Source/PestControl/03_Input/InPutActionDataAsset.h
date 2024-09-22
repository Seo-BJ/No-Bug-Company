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
	UInputAction* InputActionNum;

	UPROPERTY(EditAnywhere, Category = Input)
	UInputAction* InputActionEtc;
	
};
