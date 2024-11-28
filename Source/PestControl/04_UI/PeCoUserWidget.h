// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PeCoUserWidget.generated.h"


class UPlayerStatPresenterComponent;
class UStoreComponent;
/**
 * 
 */
UCLASS()
class PESTCONTROL_API UPeCoUserWidget : public UUserWidget
{
	GENERATED_BODY()
	


public:

	UFUNCTION(BlueprintCallable, BlueprintPure)
	UPlayerStatPresenterComponent* GetStatPresenter();	

	UFUNCTION(BlueprintCallable, BlueprintPure)
	UStoreComponent* GetStore();


};
