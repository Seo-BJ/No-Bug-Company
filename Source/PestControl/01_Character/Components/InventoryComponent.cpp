// Fill out your copyright notice in the Description page of Project Settings.


#include "01_Character/Components/InventoryComponent.h"

#include "02_Player/PeCoPlayerController.h"
#include "01_Character/PeCoPlayerCharacter.h"

#include "09_Items/PeCoItem.h"
#include "09_Items/ConsumableItem.h"
#include "09_Items/CombatItem.h"

#include "21_Data/PeCoDataTypes.h"

#include "GameFramework/PlayerState.h"
#include "Kismet/KismetArrayLibrary.h"
#include "Kismet/KismetGuidLibrary.h"
#include "Kismet/KismetSystemLibrary.h"


// Sets default values for this component's properties
UInventoryComponent::UInventoryComponent()
{

	PrimaryComponentTick.bCanEverTick = false;

	// ...
}


// Called when the game starts
void UInventoryComponent::BeginPlay()
{
	Super::BeginPlay();


	SetupInventoryStorageReference();
}

bool UInventoryComponent::AddItemsOfClass(const TSubclassOf<AActor> Class, const int32 Quantity, FText& OutNote)
{
	AActor* InventoryManagerOwner = GetOwner();
	if (!IsValid(InventoryManagerOwner))
	{
		return false;
	}

	if (Quantity <= 0)
	{
		// Failed to add item to inventory
		OutNote = FText::FromString("Quantity must be greater than zero");
		return false;
	}

	int32 StacksToAdd = Quantity;
	/* Check if inventory already has this item */
	TArray<AActor*> FilteredArray;
	const bool bAlreadyHasItem = GetAllItemsOfClass(Class, FilteredArray);

	if (bAlreadyHasItem)
	{
		for (AActor* ItemActor : FilteredArray)
		{
			APeCoItem* PeCoItem = Cast<APeCoItem>(ItemActor);
			if (!IsValid(PeCoItem))
			{
				continue;
			}

			// Is stacking items allowed?
			if (!PeCoItem->ItemInfo.bStackable)
			{
				// Failed to add item to inventory
				OutNote = FText::FromString("Trying to stack non-fungible item!");
				return false;
			}

			// Free slots available?
			const int32 FreeSlotsAvailable = FMath::Max(0, PeCoItem->ItemInfo.MaxStack - PeCoItem->ItemInfo.CurrentStack);
			if (FreeSlotsAvailable <= 0)
			{
				continue;
			}

			if (FreeSlotsAvailable >= StacksToAdd)
			{
				/* This item will add all stacks */
				PeCoItem->ItemInfo.CurrentStack = PeCoItem->ItemInfo.CurrentStack + StacksToAdd;

				/* All stacks added */
				StacksToAdd = 0;
				OnItemUpdated.Broadcast(ItemActor);
				break;
			}

			/* Maxed out stack and continue ... */
			PeCoItem->ItemInfo.CurrentStack = PeCoItem->ItemInfo.MaxStack;

			/* Decrease remaining item count and continue loop */
			StacksToAdd -= FreeSlotsAvailable;
		}

		if (StacksToAdd <= 0)
		{
			// No more work to do.
			OutNote = FText::FromString("Stacks increased to current");

			// Successfully added item to inventory.
			return true;
		}
	}

	while (StacksToAdd > 0)
	{
		/* Notice inventory storage actor is not the owner.
		 * OWNER of the inventory (preferably the pawn) is the owner and the instigator is the instigator of a new item.
		 * Player state actor is always relevant, so if we inherit always relevant for all items it wil be a multiplayer cluster fuck.
		 * Items are attached to an always relevant actor but gets net relevant with the pawn.
		 */

		UWorld* World = GetWorld();
		if (!ensure(IsValid(World)))
		{
			return false;
		}

		SetupInventoryStorageReference();
		if (!IsValid(InventoryStorage))
		{
			return false;
		}

		FActorSpawnParameters SpawnParams;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		SpawnParams.Owner = InventoryManagerOwner;
		SpawnParams.Instigator = InventoryManagerOwner->GetInstigator();

		const AActor* ItemCDO = Cast<AActor>(Class->StaticClass()->GetDefaultObject());
		FTransform NewTransform = InventoryStorage->GetActorTransform();
		NewTransform.SetScale3D(IsValid(ItemCDO) ? ItemCDO->GetActorScale() : FVector::OneVector);
		AActor* NewItemActor = World->SpawnActor(Class, &NewTransform, SpawnParams);

		/* Do some stuff relevant to item */
		APeCoItem* PeCoItem = Cast<APeCoItem>(NewItemActor);
		if (!IsValid(PeCoItem))
		{
			return false;
		}


		PeCoItem->HideShowItem(true);

		/* Attach to designated actor storage. In case of pawns, player state. (AI also has player state) */
		FAttachmentTransformRules AttachmentRules(
			EAttachmentRule::SnapToTarget,
			EAttachmentRule::SnapToTarget,
			EAttachmentRule::KeepWorld,
			false);
		NewItemActor->AttachToActor(InventoryStorage, AttachmentRules, NAME_None);

		if (PeCoItem->ItemInfo.MaxStack >= StacksToAdd)
		{
			/* Less than max? Add stacks... */
			PeCoItem->ItemInfo.CurrentStack = StacksToAdd;
			StacksToAdd = 0;
		}
		else
		{
			/* More items to spawn? Add max and reduce stacks */
			PeCoItem->ItemInfo.CurrentStack = PeCoItem->ItemInfo.MaxStack;
			StacksToAdd -= PeCoItem->ItemInfo.MaxStack;
		}
		OnItemUpdated.Broadcast(NewItemActor);
	}

	// Successfully added item to inventory
	OutNote = FText::FromString("New items spawned and registered to inventory");
	return true;
}

bool UInventoryComponent::RemoveItemsOfClass(const TSubclassOf<AActor> Class, const int32 Quantity, FText& OutNote)
{
	const AActor* InventoryManagerOwner = GetOwner();
	if (!IsValid(InventoryManagerOwner) || !InventoryManagerOwner->HasAuthority())
	{
		return false;
	}

	FText Note;
	const bool bHasEnoughItems = HasEnoughItems(
		Class,
		Quantity,
		Note);

	if (true)
	{
		const FString Msg = FString::Printf(
			TEXT("Has enough items: %s"),
			bHasEnoughItems ? TEXT("True") : TEXT("False"));
		GEngine->AddOnScreenDebugMessage(
			-1,
			2.0f,
			FColor::FromHex("00A8FFFF"),
			Msg);
		UE_LOG(LogTemp, Warning, TEXT("%s"), *Msg);
	}

	if (!bHasEnoughItems)
	{
		// Failed to remove items
		OutNote = FText::FromString("Stack to remove negative or 0");
		return false;
	}

	int32 StacksToRemove = Quantity;

	/* Has items of class in inventory. Now we just have to see if it's enough */

	TArray<AActor*> FilteredArray;
	if (!GetAllItemsOfClass(Class, FilteredArray))
	{
		// Failed to remove items
		OutNote = FText::FromString("No items of class");
		return false;
	}

	/* This item has more stacks than to remove ... */
	for (int32 i = FilteredArray.Num() - 1; i >= 0; --i)
	{
		AActor* ItemActor = FilteredArray[i];
		if (!IsValid(ItemActor))
		{
			continue;
		}
		APeCoItem* PeCoItem = Cast<APeCoItem>(ItemActor);
		if (!IsValid(PeCoItem))
		{
			continue;
		}

		if (!PeCoItem->ItemInfo.bStackable)
		{
			// Failed to remove items
			OutNote = FText::FromString("Not a fungible stackable item!");
			return false;
		}

		if (PeCoItem->ItemInfo.CurrentStack > StacksToRemove)
		{
			/* Reduce stacks and break the loop */
			PeCoItem->ItemInfo.CurrentStack -= StacksToRemove;
			StacksToRemove = 0;
		}
		else
		{
			/* Deplete all stacks, destroy item, and move along ... */
			StacksToRemove -= PeCoItem->ItemInfo.CurrentStack;
			ItemActor->Destroy();
		}
	}

	// Successfully removed items
	OutNote = FText::FromString("Successfully removed stacks");
	return true;
}


TArray<AActor*> UInventoryComponent::GetAllItems()
{
	TArray<AActor*> Items;

	SetupInventoryStorageReference();
	if (!IsValid(InventoryStorage))
	{
		return Items;
	}

	InventoryStorage->GetAttachedActors(Items, true);

	/* Reverse loop as if we do normal loop.
	 * By deleting an index, the next item immediately takes its place so we skip one item
	 */
	for (int32 i = Items.Num() - 1; i >= 0; --i)
	{
		const AActor* ItemActor = Items[i];

		if (!ItemActor->ActorHasTag(APeCoItem::TAG_ITEM))
		{
			Items.RemoveAt(i);
			continue;
		}
	}

	return Items;
}
bool UInventoryComponent::GetAllItemsOfClass(const TSubclassOf<AActor> Class, TArray<AActor*>& OutFilteredArray)
{
	TArray<AActor*> FilteredArray;
	UKismetArrayLibrary::FilterArray(GetAllItems(), Class, FilteredArray);
	if (FilteredArray.Num() > 0)
	{
		OutFilteredArray = FilteredArray;
		return true;
	}

	return false;
}

bool UInventoryComponent::HasEnoughItems(const TSubclassOf<AActor> Item, const int32 Quantity, UPARAM(DisplayName = "Note") FText& OutNote)
{
	/* Do this check before crafting to see if reduce stack will succeed */

	if (Quantity <= 0)
	{
		OutNote = FText::FromString("Not enough Items. Quantity must be greater than zero");
		return false;
	}

	int32 QuantityMissing = Quantity;

	TArray<AActor*> FilteredArray;
	if (!GetAllItemsOfClass(Item, FilteredArray))
	{
		OutNote = FText::FromString("Has enough check failed: No items of such class found");
		return false;
	}

	for (int32 i = 0; i < FilteredArray.Num(); ++i)
	{
		const AActor* ItemActor = FilteredArray[i];

		const APeCoItem* PeCoItem = Cast<APeCoItem>(ItemActor);
		if (!ensure(IsValid(PeCoItem)))
		{
			continue;
		}

		/* Reduce quantity seeking */
		QuantityMissing -= PeCoItem->ItemInfo.CurrentStack;

		if (QuantityMissing <= 0)
		{
			/* Got enough */
			break;
		}
	}

	if (QuantityMissing <= 0)
	{
		OutNote = FText::FromString("Success! Got enough items");
		return true;
	}

	OutNote = FText::FromString("Failed. Not enough items");
	return false;
}









 UPARAM(DisplayName = "Success") bool UInventoryComponent::EquipItemInSlot(const FName Slot, AActor* ItemActor, AActor*& OutPreviousItem, AActor*& OutNewItem)
 {



	 return UPARAM(DisplayName = "Success") bool();
 }


 /*
 
 void UInventoryComponent::UseItemInQuickSlot(EItemType ItemType)
{
	if (ItemType == EItemType::EItemType_Consumable && QuickSlotConsumableItem)
	{
		QuickSlotConsumableItem->UseItem();
	}
	else if (ItemType == EItemType::EItemType_Consumable && QuickSlotCombatItem)
	{
		QuickSlotCombatItem->UseItem();
	}
}

APeCoItem* UInventoryComponent::CycleItemSlot(EItemType ItemType, bool bIndexUp)
{
	if (ItemType == EItemType::EItemType_Consumable)
	{
		int32 CurrentIndex = ConsumableItems.Find(QuickSlotConsumableItem);
		int32 Num = ConsumableItems.Num();
		int32 NextIndex = bIndexUp == true? FMath::Clamp(CurrentIndex + 1, 0, Num) : FMath::Clamp(CurrentIndex - 1, 0, Num);
		SetItemInQuickSlot(EItemType::EItemType_Consumable, ConsumableItems[NextIndex]);
		return QuickSlotConsumableItem;
	}
	else if (ItemType == EItemType::EItemType_Combat)
	{
		int32 CurrentIndex = CombatItems.Find(QuickSlotCombatItem);
		int32 Num = CombatItems.Num();
		int32 NextIndex = bIndexUp == true ? FMath::Clamp(CurrentIndex + 1, 0, Num) : FMath::Clamp(CurrentIndex - 1, 0, Num);
		SetItemInQuickSlot(EItemType::EItemType_Combat, CombatItems[NextIndex]);
		return QuickSlotCombatItem;
	}
	return nullptr;
}

void UInventoryComponent::SetItemInQuickSlot(EItemType ItemType, APeCoItem* Item)
{
	if (ItemType == EItemType::EItemType_Consumable)
	{
		QuickSlotConsumableItem = Cast<AConsumableItem>(Item);
		UE_LOG(LogTemp, Warning, TEXT("%s"), *QuickSlotConsumableItem->GetName());
	}
	else if (ItemType == EItemType::EItemType_Combat)
	{
		QuickSlotCombatItem = Cast<ACombatItem>(Item);
		UE_LOG(LogTemp, Warning, TEXT("%s"), *QuickSlotCombatItem->GetName());
	}
}
 
 */



void UInventoryComponent::SetupInventoryStorageReference()
{
	if (IsValid(InventoryStorage))
	{
		// Storage ref already set up
		return;
	}
	AActor* InventoryOwner = GetOwner();
	if (!IsValid(InventoryOwner))
	{
		return;
	}
	bool bHasPlayerState = Cast<APawn>(GetOwner()) != nullptr;
	if (bHasPlayerState)
	{
		// Try to get a ref to the player state.
		const APawn* OwningPawn = Cast<APawn>(InventoryOwner);
		if (!IsValid(OwningPawn))
		{
			return;
		}
		AActor* PlayerState = OwningPawn->GetPlayerState();
		if (IsValid(PlayerState))
		{
			InventoryStorage = PlayerState;
		}
	}
	else
	{
		InventoryStorage = InventoryOwner;
	}
}

/*

void UInventoryComponent::AddItem(EItemType ItemType, APeCoItem* Item)
{
	if (ItemType == EItemType::EItemType_Consumable)
	{

		ConsumableItems.Add(Cast<AConsumableItem>(Item));
		if (QuickSlotConsumableItem == nullptr)
		{
			SetItemInQuickSlot(ItemType, Item);
		}
	}
	else if (ItemType == EItemType::EItemType_Combat)
	{
		CombatItems.Add(Cast<ACombatItem>(Item));
		if (QuickSlotCombatItem == nullptr)
		{
			SetItemInQuickSlot(ItemType, Item);
		}
	}
}


*/

