// Fill out your copyright notice in the Description page of Project Settings.


#include "09_Items/PeCoItem.h"




#include "01_Character/Components/InventoryComponent.h"


const FName APeCoItem::TAG_ITEM = FName("Item");



FPeCoItemInfo::FPeCoItemInfo()
{


}



// Sets default values
APeCoItem::APeCoItem()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

}


// Called when the game starts or when spawned
void APeCoItem::BeginPlay()
{
	Super::BeginPlay();
	
}


void APeCoItem::PickUpItem(UInventoryComponent* Inventory)
{
    if (!IsValid(Inventory))
    {
        return;
    }

    AActor* InventoryOwner = Inventory->GetOwner();
    if (!IsValid(InventoryOwner))
    {
        return;
    }


    /* Different logic for stacking */
    if (ItemInfo.bStackable)
    {
        /* Stack items in inventory */

        FText Note;
        const bool bSuccess = Inventory->AddItemsOfClass(GetClass(), ItemInfo.CurrentStack, Note);

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
            SetOwner(InventoryOwner);
            SetInstigator(InventoryOwner->GetInstigator());
            // OnPickup.Broadcast(Inventory);
            Destroy();
        }
    }




}

void APeCoItem::UseItem()
{
    // OnItemUsed.Broadcast(User, GameplayTag);
}











void APeCoItem::HideShowItem(const bool bHide)
{
    if (bHide)
    {
        UPrimitiveComponent* PrimitiveComponent = Cast<UPrimitiveComponent>(GetRootComponent());
        if (IsValid(PrimitiveComponent))
        {
            PrimitiveComponent->SetSimulatePhysics(false);
        }
    }
    else
    {
        const FDetachmentTransformRules DetachmentRules(EDetachmentRule::KeepWorld, true);
        DetachFromActor(DetachmentRules);
    }

    SetActorHiddenInGame(bHide);
    SetActorEnableCollision(!bHide);

    OnHiddenShown.Broadcast(bHide);
}

void APeCoItem::EquipInternal(const FName Slot)
{
    AActor* ItemActor = GetOwner();
    if (!IsValid(ItemActor) || !ItemActor->HasAuthority())
    {
        return;
    }

    AActor* ParentItemActor = ItemActor->GetOwner();
    if (!IsValid(ParentItemActor))
    {
        return;
    }

    const FDetachmentTransformRules DetachmentRules(EDetachmentRule::KeepWorld, true);
    ItemActor->DetachFromActor(DetachmentRules);

    HideShowItem(false);

    UPrimitiveComponent* PrimitiveComponent = Cast<UPrimitiveComponent>(ItemActor->GetRootComponent());
    if (IsValid(PrimitiveComponent))
    {
        PrimitiveComponent->SetSimulatePhysics(false);
    }

    OnEquip.Broadcast(ParentItemActor, Slot);

}

void APeCoItem::UnEquipInternal(const FName Slot)
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

    const FAttachmentTransformRules AttachmentRules(
        EAttachmentRule::SnapToTarget,
        EAttachmentRule::SnapToTarget,
        EAttachmentRule::KeepWorld,
        false);
    ItemComponentOwner->AttachToActor(Inventory->InventoryStorage, AttachmentRules, NAME_None);
    HideShowItem(true);
    OnUnEquip.Broadcast(ItemOwner, Slot);
}


