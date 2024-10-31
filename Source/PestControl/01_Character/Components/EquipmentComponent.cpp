// Fill out your copyright notice in the Description page of Project Settings.


#include "EquipmentComponent.h"

#include "01_Character/Components/InventoryComponent.h"
#include "09_Items/PeCoItem.h"

UEquipmentComponent::UEquipmentComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

	// ...
}
void UEquipmentComponent::BeginPlay()
{
	Super::BeginPlay();
	
}


 bool UEquipmentComponent::GetAllItems(TArray<AActor*>& OutItems)
{
     OutItems.Reset(EquipmentList.Num());

     for (const FEquipmentInfo& EquipmentItem : EquipmentList)
     {
         if (IsValid(EquipmentItem.ItemActor))
         {
             OutItems.Add(EquipmentItem.ItemActor);
         }
     }
     return OutItems.Num() > 0;
}

bool UEquipmentComponent::EquipItemInSlot(const FName Slot, AActor* ItemActor, AActor*& OutPreviousItem, AActor*& OutNewItem)
{
    if (!IsValid(GetOwner()) || !IsValid(ItemActor))
    {
        return false;
    }

    // 이미 같은 아이템이 장착된 경우 조기 return
    AActor* ItemInSlot;
    if (GetItemInSlot(Slot, ItemInSlot))
    {
        if (ItemInSlot == ItemActor)
        {
            // Same item is already equipped
            return false;
        }
    }

    for (int32 i = 0; i < EquipmentList.Num(); ++i)
    {
        const FEquipmentInfo& EquipmentElement = EquipmentList[i];

        if (Slot != EquipmentElement.Id)
        {
            continue;
        }

        if (!ItemActor->ActorHasTag(APeCoItem::TAG_ITEM))
        {
            continue;
        }

        APeCoItem* PeCoItem = Cast<APeCoItem>(ItemActor);
        if (!IsValid(PeCoItem))
        {
            continue;
        }

        const bool bMatches = PeCoItem->ItemInfo.ItemType == EquipmentElement.AcceptableSlotType;
        if (!bMatches)
        {
            continue;
        }

        // All conditions are met -> Start to equip item

        // Skip UnEquip if item is NOT valid (empty slot!)
        if (IsValid(EquipmentElement.ItemActor))
        {
            OutPreviousItem = EquipmentElement.ItemActor;
            APeCoItem* OutPeCoItem = Cast<APeCoItem>(OutPreviousItem);
            if (IsValid(OutPeCoItem))
            {
                OutPeCoItem->UnEquipInternal(Slot);
            }
        }

        EquipmentList[i].ItemActor = ItemActor;
        OutNewItem = ItemActor;

        PeCoItem->EquipInternal(Slot);

        // Item equipped successfully
        return true;
    }

    // Failed to equip item
    return false;
}

bool UEquipmentComponent::UnEquipItemFromSlot(const FName Slot, AActor*& OutItemUnequipped)
{
    if (!IsValid(GetOwner()))
    {
        return false;
    }

    for (int32 i = 0; i < EquipmentList.Num(); ++i)
    {
        FEquipmentInfo& EquipmentElement = EquipmentList[i];

        if (Slot != EquipmentElement.Id)
        {
            continue;
        }

        if (!IsValid(EquipmentElement.ItemActor))
        {
            continue;
        }

        /* slot exists + item is an item + tag is acceptable in this slot */

        OutItemUnequipped = EquipmentElement.ItemActor;
        EquipmentElement.ItemActor = nullptr;

        APeCoItem* OutPeCoItem = Cast<APeCoItem>(OutItemUnequipped);
        if (IsValid(OutPeCoItem))
        {
            OutPeCoItem->UnEquipInternal(Slot);
        }

        // Item unequipped successfully
        return true;
    }
    // Failed to UnEquip item
    return false;
}
bool UEquipmentComponent::GetItemInSlot(const FName Slot, AActor*& OutItem)
{
    for (const FEquipmentInfo& EquipmentElement : EquipmentList)
    {
        if (EquipmentElement.Id != Slot)
        {
            continue;
        }

        // Found matching slot ID

        if (!IsValid(EquipmentElement.ItemActor))
        {
            // Failed to find item that is also valid
            return false;
        }

        OutItem = EquipmentElement.ItemActor;

        // Valid item found
        return true;
    }

    // Failed to find item
    return false;
}



