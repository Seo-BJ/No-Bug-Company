// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "09_Items/Components/PeCoItemComponent.h"
#include "CombatItemComponent.generated.h"

/**
 * 
 */
UCLASS(BlueprintType, Blueprintable, ClassGroup = ("InventorySystem"), meta = (BlueprintSpawnableComponent))
class PESTCONTROL_API UCombatItemComponent : public UPeCoItemComponent
{
	GENERATED_BODY()

public:


	UFUNCTION(BlueprintCallable,Category = "Item")
	virtual bool UseItem(AActor* User) override;

	
};
