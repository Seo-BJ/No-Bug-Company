// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "04_UI/PeCoUserWidget.h"
#include "PeCoProgressBar.generated.h"

class UProgressBar;
/**
 * 
 */
UCLASS()
class PESTCONTROL_API UPeCoProgressBar : public UPeCoUserWidget
{
	GENERATED_BODY()

public:

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	class UProgressBar* ProgressBar;



	
};
