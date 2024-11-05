// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "09_Items/Components/PeCoItemComponent.h"
#include "ConsumtionItemComponent.generated.h"

/**
 * 
 */
UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class PESTCONTROL_API UConsumtionItemComponent : public UPeCoItemComponent
{
	GENERATED_BODY()


public:

	UFUNCTION(BlueprintCallable, Category = "Item")
	virtual bool UseItem(AActor* User) override;

	UFUNCTION(BlueprintNativeEvent, Category = "Item")
	void ConsumeItem();

};
