// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "InventoryComponent.generated.h"


class APotion;
class AItem;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PESTCONTROL_API UInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UInventoryComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	// Selected Weapon (single item)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
	TObjectPtr<AActor> SelectedWeapon;

	// Potions (multiple items, so we use an array)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
	TObjectPtr<APotion> Potions;

	// Other Items (multiple items, so we use an array)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
	uint8 PotionCounts;

	// A map to store the item count for each type of item
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
	TMap<TObjectPtr<AItem>, int32> ItemCounts;
		
};
