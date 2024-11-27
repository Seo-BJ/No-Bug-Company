// Fill out your copyright notice in the Description page of Project Settings.


#include "PeCoItemComponent.h"
#include "01_Character/PeCoPlayerCharacter.h"
#include "01_Character/Components/InventoryComponent.h"
#include "01_Character/Components/EquipmentComponent.h"

#include "09_Items/CombatItem.h"

#include "20_System/PeCoFunctionLibrary.h"

FPeCoItemInfo::FPeCoItemInfo()
{


}
UPeCoItemComponent::UPeCoItemComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
}
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

bool UPeCoItemComponent::UseItem(AActor* User, bool bNeedSpawn)
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
    if (ItemInfo.CurrentQuantity <= 0)
    {
        return false;
    }
    if (bNeedSpawn)
    {
        AActor* SpawnedActor = nullptr;
        SpawnItemForUse(SpawnedActor);
        ItemInfo.CurrentQuantity -= 1;
        UPeCoItemComponent* SpawnedItemComponent = UPeCoFunctionLibrary::GetItemComponent(SpawnedActor);
        if (IsValid(SpawnedItemComponent))
        {
            SpawnedItemComponent->HideShowItem(false);
            SpawnedItemComponent->OnItemUsed.Broadcast(User);
            return true;
        }
    }
    else
    {
        ItemInfo.CurrentQuantity -= 1;
        OnItemUsed.Broadcast(User);
        return true;
    }
    /*
    UPrimitiveComponent* PrimitiveComponent = Cast<UPrimitiveComponent>(ItemActor->GetRootComponent());
    if (IsValid(PrimitiveComponent))
    {
        PrimitiveComponent->SetSimulatePhysics(false);
    }
    */
    return false;
}

bool UPeCoItemComponent::SpawnItemForUse(AActor*& OutSpawnActor)
{
    UWorld* World = GetWorld();
    if (!ensure(IsValid(World)))
    {
        return false;
    }
    AActor* ItemActor = GetOwner();
    if (!IsValid(ItemActor))
    {
        return false;
    }
    AActor* ItemActorOwner = ItemActor->GetOwner();
    if (!IsValid(ItemActorOwner))
    {
        return false;
    }
    UInventoryComponent* InventoryManager = UPeCoFunctionLibrary::GetInventoryComponent(ItemActorOwner);
    if (!IsValid(InventoryManager))
    {
        return false;
    }
    APeCoPlayerCharacter* PlayerCharacter = CastChecked<APeCoPlayerCharacter>(ItemActorOwner);
    if (!IsValid(PlayerCharacter))
    {
        return false;
    }
    const AActor* ItemCDO = Cast<AActor>(ItemActor->StaticClass()->GetDefaultObject());

    FActorSpawnParameters SpawnParams;
    SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
    SpawnParams.Owner = ItemActorOwner;
    SpawnParams.Instigator = ItemActorOwner->GetInstigator();

    FTransform NewTransform = ItemActor->GetActorTransform();
    if (ItemInfo.bSimulateWhenDropped)
    {
        NewTransform.SetLocation(PlayerCharacter->GetWeaponSpawnPointLocation());
    }
    else
    {
        FVector StartLocation = PlayerCharacter->GetWeaponSpawnPointLocation();
        FVector EndLocation = StartLocation - FVector(0.f, 0.f, 1000.f); 
        FHitResult HitResult;
        FCollisionQueryParams TraceParams(FName(TEXT("SpawnItemTrace")), true, ItemActorOwner);
        FVector SpawnLocation = StartLocation; 
        if (World->LineTraceSingleByChannel(HitResult, StartLocation, EndLocation, ECC_Visibility, TraceParams))
        {
            if (HitResult.bBlockingHit)
            {
                SpawnLocation = HitResult.ImpactPoint + FVector(0.f, 0.f, 10.f); 
            }
        }
        NewTransform.SetLocation(SpawnLocation);
    }
    NewTransform.SetScale3D(IsValid(ItemCDO) ? ItemCDO->GetActorScale() : FVector::OneVector);
    OutSpawnActor = World->SpawnActor(ItemActor->GetClass(), &NewTransform, SpawnParams);
    return true;
}

void UPeCoItemComponent::HideShowItem(const bool bHide)
{
    AActor* ItemComponentOwner = GetOwner();
    if (!IsValid(ItemComponentOwner))
    {
        return;
    }
    UPrimitiveComponent* PrimitiveComponent = Cast<UPrimitiveComponent>(ItemComponentOwner->GetRootComponent());
    if (!IsValid(PrimitiveComponent))
    {
        return;
    }
    if (bHide)
    {
        PrimitiveComponent->SetSimulatePhysics(false);
    }
    else
    {
        const FDetachmentTransformRules DetachmentRules(EDetachmentRule::KeepWorld, true);
        ItemComponentOwner->DetachFromActor(DetachmentRules);
        PrimitiveComponent->SetSimulatePhysics(ItemInfo.bSimulateWhenDropped);
    }

    ItemComponentOwner->SetActorHiddenInGame(bHide);
    ItemComponentOwner->SetActorEnableCollision(!bHide);
}

void UPeCoItemComponent::EquipInternal(const FGameplayTag SlotTag) const
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


    OnEquip.Broadcast(ParentItemActor, SlotTag);

}
void UPeCoItemComponent::UnEquipInternal(const FGameplayTag SlotTag) const
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

    OnUnEquip.Broadcast(ItemOwner, SlotTag);
}


