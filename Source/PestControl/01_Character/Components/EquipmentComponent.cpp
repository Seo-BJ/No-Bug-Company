// Fill out your copyright notice in the Description page of Project Settings.


#include "EquipmentComponent.h"

#include "01_Character/Components/InventoryComponent.h"
#include "02_Player/PeCoPlayerController.h"
#include "09_Items/Components/PeCoItemComponent.h"
#include "20_System/PeCoFunctionLibrary.h"

UEquipmentComponent::UEquipmentComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

	// ...
}
void UEquipmentComponent::BeginPlay()
{
	Super::BeginPlay();
	
}

bool UEquipmentComponent::UseItemInSlot(const ESlotType SlotType)
{
    AActor* ItemInSlot;
    if (GetItemInSlot(SlotType, ItemInSlot))
    {
        UPeCoItemComponent* ItemComponent = UPeCoFunctionLibrary::GetItemComponent(ItemInSlot);
        if (ItemComponent && ItemComponent->GetOwner())
        {
            ItemComponent->UseItem(GetOwner());
        }
        OnItemUsedInSlot.Broadcast(ItemInSlot, SlotType);
        return true;
    }
    return false;
}


bool UEquipmentComponent::EquipItemInSlot(const ESlotType SlotType, AActor* ItemActor, AActor*& OutPreviousItem, AActor*& OutNewItem)
{
    if (!IsValid(GetOwner()) || !IsValid(ItemActor))
    {
        return false;
    }

    // 이미 같은 아이템이 장착된 경우 조기 return
    AActor* ItemInSlot;
    if (GetItemInSlot(SlotType, ItemInSlot))
    {
        if (ItemInSlot == ItemActor)
        {
            return false;
        }
    }

    for (int32 i = 0; i < EquipmentList.Num(); ++i)
    {
        const FEquipmentInfo& EquipmentElement = EquipmentList[i];

        if (SlotType != EquipmentElement.CurrentSlot)
        {
            continue;
        }
        if (!ItemActor->ActorHasTag(UPeCoItemComponent::TAG_ITEM))
        {
            continue;
        }
        const UPeCoItemComponent* ItemComponent = UPeCoFunctionLibrary::GetItemComponent(ItemActor);
        if (!IsValid(ItemComponent))
        {
            continue;
        }
        const bool bMatches = ItemComponent->ItemInfo.SlotType == EquipmentElement.AcceptableSlotType;
        if (!bMatches)
        {
            continue;
        }

        // 아이템 장착 시작

        // Skip UnEquip if item is NOT valid (empty slot!)
        if (IsValid(EquipmentElement.ItemActor))
        {
            OutPreviousItem = EquipmentElement.ItemActor;
            const UPeCoItemComponent* PreviousItemComponent = UPeCoFunctionLibrary::GetItemComponent(OutPreviousItem);
            if (IsValid(PreviousItemComponent))
            {
                PreviousItemComponent->UnEquipInternal(SlotType);
            }
        }

        EquipmentList[i].ItemActor = ItemActor;
        OutNewItem = ItemActor;

        ItemComponent->EquipInternal(SlotType);

        // Item equipped successfully


        OnItemEquip.Broadcast(OutNewItem, SlotType);
        return true;
    }

    // Failed to equip item
    return false;
}
bool UEquipmentComponent::UnEquipItemFromSlot(const ESlotType SlotType, AActor*& OutItemUnequipped)
{
    if (!IsValid(GetOwner()))
    {
        return false;
    }

    for (int32 i = 0; i < EquipmentList.Num(); ++i)
    {
        FEquipmentInfo& EquipmentElement = EquipmentList[i];

        if (SlotType != EquipmentElement.CurrentSlot)
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

        const UPeCoItemComponent* UnequippedItemComponent = UPeCoFunctionLibrary::GetItemComponent(OutItemUnequipped);
        if (IsValid(UnequippedItemComponent))
        {
            UnequippedItemComponent->UnEquipInternal(SlotType);
        }

        // Item unequipped successfully
        OnItemUnEquip.Broadcast(OutItemUnequipped, SlotType);
        return true;
    }
    // Failed to UnEquip item
    return false;
}

bool UEquipmentComponent::GetItemInSlot(const ESlotType SlotType, AActor*& OutItem)
{
    for (const FEquipmentInfo& EquipmentElement : EquipmentList)
    {
        if (EquipmentElement.CurrentSlot != SlotType)
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


