// Fill out your copyright notice in the Description page of Project Settings.


#include "01_Character/Components/InventoryComponent.h"

#include "02_Player/PeCoPlayerController.h"
#include "01_Character/PeCoPlayerCharacter.h"

#include "09_Items/PeCoItem.h"
#include "09_Items/Potion.h"

// Sets default values for this component's properties
UInventoryComponent::UInventoryComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UInventoryComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UInventoryComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UInventoryComponent::AddItemToInveotry(APeCoItem* Item, uint32 Amount)
{
	if (Item)
	{
		EConsumableItemType ItemType = Item->GetItemType();
		{
			if (PossessedConsumableItem.Contains(ItemType))
			{
				PossessedConsumableItem[ItemType] = PossessedConsumableItem[ItemType] + Amount;
				if (ItemType == CurrentItemTypeInSlot)
				{
					UpdateItemSlot(ItemType);
				}
			}
		}
	}
}

void UInventoryComponent::UpdateItemSlot(EConsumableItemType ItemType)
{
	if (PossessedConsumableItem.Contains(ItemType))
	{
		uint32 ItemAmount = PossessedConsumableItem[ItemType];
		APeCoPlayerCharacter* Character = Cast<APeCoPlayerCharacter>(GetOwner());
		if (Character)
		{
			PlayerController = PlayerController == nullptr ? Cast<APeCoPlayerController>(Character->GetController()) : PlayerController;
			if (PlayerController)
			{
				PlayerController->SetHUDItemSlotCount(ItemType, ItemAmount);
			}
		}
	}
}

void UInventoryComponent::ChangeItemInSlot(APeCoItem* NewItem)
{




}


