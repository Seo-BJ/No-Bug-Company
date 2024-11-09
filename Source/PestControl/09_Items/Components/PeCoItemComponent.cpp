// Fill out your copyright notice in the Description page of Project Settings.


#include "PeCoItemComponent.h"

#include "01_Character/Components/InventoryComponent.h"
#include "01_Character/Components/EquipmentComponent.h"


const FName UPeCoItemComponent::TAG_ITEM = FName("Item");



FPeCoItemInfo::FPeCoItemInfo()
{


}



// Sets default values
UPeCoItemComponent::UPeCoItemComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
}


// Called when the game starts or when spawned
void UPeCoItemComponent::BeginPlay()
{
    Super::BeginPlay();
    AActor* ItemComponentOwner = GetOwner();
    if (!ensure(IsValid(ItemComponentOwner)))
    {
        return;
    }

    /* We must use owner relevancy to get loaded into memory with the owner */
    ItemComponentOwner->bNetUseOwnerRelevancy = true;

    /* Tag item for easier queries - tags are not replicated so watch out what is "server=true" and what is just begin play */
    ItemComponentOwner->Tags.AddUnique(TAG_ITEM);

}


void UPeCoItemComponent::PickUpItem(UInventoryComponent* Inventory)
{
    AActor* InventoryOwner = Inventory->GetOwner();
    if (!IsValid(InventoryOwner))
    {
        return;
    }
    AActor* ItemActor = GetOwner();
    if (!IsValid(ItemActor))
    {
        return;
    }
    if (!IsValid(Inventory))
    {
        return;
    }
    /* Different logic for stacking */
    if (ItemInfo.bStackable)
    {
        /* Stack items in inventory */

        FText Note;
        const bool bSuccess = Inventory->AddItemsOfClass(GetClass(), ItemInfo.CurrentQuantity, Note);

        if (true)
        {
            const FString Msg = FString::Printf(
                TEXT("Pick up item %s -- %s"),
                bSuccess ? TEXT("True") : TEXT("False"),
                *Note.ToString());
            GEngine->AddOnScreenDebugMessage(
                -1,
                2.0f,
                FColor::FromHex("00A8FFFF"),
                Msg);
            UE_LOG(LogTemp, Warning, TEXT("%s"), *Msg);
        }

        if (bSuccess)
        {
            // Set ownership and instigator for the item - so the dispatcher can notify the owner of the update.
            ItemActor->SetOwner(InventoryOwner);
            ItemActor->SetInstigator(InventoryOwner->GetInstigator());
            // OnPickup.Broadcast(InventoryPicking);
            ItemActor->Destroy();
        }
    }
}

bool UPeCoItemComponent::UseItem(AActor* User)
{
    AActor* ItemActor = GetOwner();
    if (!IsValid(ItemActor))
    {
        return false;
    }
    // ParentItemActor = PlayerCharacter
    AActor* ParentItemActor = ItemActor->GetOwner();
    if (!IsValid(ParentItemActor))
    {
        return false;
    }

    HideShowItem(false);
    UPrimitiveComponent* PrimitiveComponent = Cast<UPrimitiveComponent>(ItemActor->GetRootComponent());
    if (IsValid(PrimitiveComponent))
    {
        PrimitiveComponent->SetSimulatePhysics(false);
    }

    if (ItemInfo.CurrentQuantity > 0)
    {
        ItemInfo.CurrentQuantity -= 1;
        OnItemUsed.Broadcast(User);
        return true;
    }
    return false;
}

void UPeCoItemComponent::HideShowItem(const bool bHide) const
{
    AActor* ItemComponentOwner = GetOwner();
    if (!IsValid(ItemComponentOwner))
    {
        return;
    }

    if (bHide)
    {
        UPrimitiveComponent* PrimitiveComponent = Cast<UPrimitiveComponent>(ItemComponentOwner->GetRootComponent());
        if (IsValid(PrimitiveComponent))
        {
            PrimitiveComponent->SetSimulatePhysics(false);
        }
    }
    else
    {
        const FDetachmentTransformRules DetachmentRules(EDetachmentRule::KeepWorld, true);
        ItemComponentOwner->DetachFromActor(DetachmentRules);
    }

    ItemComponentOwner->SetActorHiddenInGame(bHide);
    ItemComponentOwner->SetActorEnableCollision(!bHide);
}

void UPeCoItemComponent::EquipInternal(const ESlotType SlotType) const
{
    AActor* ItemActor = GetOwner();
    if (!IsValid(ItemActor))
    {
        return;
    }
    // ParentItemActor = PlayerCharacter
    AActor* ParentItemActor = ItemActor->GetOwner();
    if (!IsValid(ParentItemActor))
    {
        return;
    }

    /*
    const FDetachmentTransformRules DetachmentRules(EDetachmentRule::KeepWorld, true);
    ItemActor->DetachFromActor(DetachmentRules);

    HideShowItem(false);

    UPrimitiveComponent* PrimitiveComponent = Cast<UPrimitiveComponent>(ItemActor->GetRootComponent());
    if (IsValid(PrimitiveComponent))
    {
        PrimitiveComponent->SetSimulatePhysics(false);
    }
    
    */


    OnEquip.Broadcast(ParentItemActor, SlotType);

}
void UPeCoItemComponent::UnEquipInternal(const ESlotType SlotType) const
{
    AActor* ItemComponentOwner = GetOwner();
    if (!IsValid(ItemComponentOwner) || !ItemComponentOwner->HasAuthority())
    {
        return;
    }
    AActor* ItemOwner = ItemComponentOwner->GetOwner();
    if (!IsValid(ItemOwner))
    {
        return;
    }
    UInventoryComponent* Inventory = Cast<UInventoryComponent>(ItemOwner->GetComponentByClass(UInventoryComponent::StaticClass()));
    if (!IsValid(Inventory))
    {
        return;
    }
    Inventory->SetupInventoryStorageReference();
    if (!IsValid(Inventory->InventoryStorage))
    {
        return;
    }

    /*
        const FAttachmentTransformRules AttachmentRules(
        EAttachmentRule::SnapToTarget,
        EAttachmentRule::SnapToTarget,
        EAttachmentRule::KeepWorld,
        false);
    ItemComponentOwner->AttachToActor(Inventory->InventoryStorage, AttachmentRules, NAME_None);
    HideShowItem(true);
    
    */

    OnUnEquip.Broadcast(ItemOwner, SlotType);
}


