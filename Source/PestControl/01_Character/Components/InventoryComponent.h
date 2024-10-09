// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "21_Data/PeCoDataTypes.h"

#include "InventoryComponent.generated.h"


class APeCoPlayerController;

class APotion;
class APeCoItem;

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

	//~Weapon

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
	TObjectPtr<AActor> SelectedWeapon;

	//~End of Weapon

	//~Item Inventory

	TMap<EItemType, uint32> CarriedItemMap;

	void AddItemToInveotry(APeCoItem* Item, uint32 Counts);

	void UpdateItemSlot(EItemType ItemType);

	void ChangeItemInSlot(APeCoItem* NewItem);

	EItemType CurrentItemTypeInSlot;

	//~End of Item Inventory

private:

	TObjectPtr<APeCoPlayerController> PlayerController;


		
};
