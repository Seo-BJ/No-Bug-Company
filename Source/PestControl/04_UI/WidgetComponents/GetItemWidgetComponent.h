// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/WidgetComponent.h"
#include "PeCoGameplayTags.h"
#include "GetItemWidgetComponent.generated.h"

/**
 * 
 */
UCLASS()
class PESTCONTROL_API UGetItemWidgetComponent : public UWidgetComponent
{
	GENERATED_BODY()
	
public:

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void SetGetItemText(FGameplayTag ItemTag, int32 Quantity);
};
