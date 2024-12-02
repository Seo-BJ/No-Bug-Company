// Fill out your copyright notice in the Description page of Project Settings.


#include "InventoryComponent.h"

#include "01_Character/PeCoPlayerCharacter.h"
#include "02_Player/PeCoPlayerController.h"
#include "09_Items/Components/PeCoItemComponent.h"
#include "20_System/PeCoFunctionLibrary.h"
#include "21_Data/PeCoDataTypes.h"

#include "GameFramework/PlayerState.h"
#include "Kismet/KismetArrayLibrary.h"
#include "Kismet/KismetGuidLibrary.h"
#include "Kismet/KismetSystemLibrary.h"


UInventoryComponent::UInventoryComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}
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
		OutNote = FText::FromString("아이템의 Owner가 유효하지 않음");
		return false;
	}

	if (Quantity <= 0)
	{
		// Failed to add item to inventory
		OutNote = FText::FromString("Quantity는 반드시 0보다 커야함.");
		return false;
	}

	/* Check if inventory already has this item */
	AActor* FiteredActor;
	const bool bAlreadyHasItem = GetItemOfClass(Class, FiteredActor);

	// 이미 해당 아이템을 가지고 있는 경우
	if (bAlreadyHasItem)
	{
		UPeCoItemComponent* ItemComponent = UPeCoFunctionLibrary::GetItemComponent(FiteredActor);
		if (!IsValid(ItemComponent))
		{
			OutNote = FText::FromString("해당 Item에 ItemComponent가 존재하지 않음.");
			return false;
		}

		if (!ItemComponent->ItemInfo.bStackable)
		{
			// Failed to add item to inventory
			OutNote = FText::FromString("해당 아이템은 1개만 가질 수 있음.");
			return false;
		}

		ItemComponent->ItemInfo.CurrentQuantity += Quantity;
		OutNote = FText::FromString("아이템 개수 증가.");
		OnItemUpdated.Broadcast(FiteredActor);

		return true;
	}

	// 아이템을 가지고 있지 않는 경우
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

	UPeCoItemComponent* NewItemComponent = UPeCoFunctionLibrary::GetItemComponent(NewItemActor);
	if (!IsValid(NewItemComponent))
	{
		return false;
	}

	NewItemComponent->HideShowItem(true);

	FAttachmentTransformRules AttachmentRules(
		EAttachmentRule::SnapToTarget,
		EAttachmentRule::SnapToTarget,
		EAttachmentRule::KeepWorld,
		false);
	NewItemActor->AttachToActor(InventoryStorage, AttachmentRules, NAME_None);

	// 전체 Quantity를 새 아이템에 설정
	NewItemComponent->ItemInfo.CurrentQuantity = Quantity;
	OnItemUpdated.Broadcast(NewItemActor);

	OutNote = FText::FromString("New item spawned and registered to inventory");
	return true;
}
bool UInventoryComponent::RemoveItemsOfClass(const TSubclassOf<AActor> Class, const int32 Quantity, FText& OutNote)
{
	const AActor* InventoryManagerOwner = GetOwner();
	if (!IsValid(InventoryManagerOwner))
	{
		return false;
	}

	FText Note;
	const bool bHasEnoughItems = HasEnoughItems(Class, Quantity, Note);
	if (!bHasEnoughItems)
	{
		// Failed to remove items
		OutNote = FText::FromString("제거할 만큼 충분한 아이템을 가지고 있지 않음.");
		return false;
	}

	AActor* FiltertedActor;
	if (!GetItemOfClass(Class, FiltertedActor))
	{
		OutNote = FText::FromString("제거하려는 Class의 아이템이 존재하지 않음.");
		return false;
	}
	if (!IsValid(FiltertedActor))
	{
		OutNote = FText::FromString("제거하려는 아이템이 유효하지 않음.");
		return false;
	}
	UPeCoItemComponent* ItemComponent = UPeCoFunctionLibrary::GetItemComponent(FiltertedActor);
	if (!IsValid(ItemComponent))
	{
		OutNote = FText::FromString("제거하려는 아이템에 ItemComponent가 유효하지 않음.");
		return false;
	}

	if (!ItemComponent->ItemInfo.bStackable)
	{
		// Failed to remove items
		OutNote = FText::FromString("Not a fungible stackable item!");
		return false;
	}

	if (ItemComponent->ItemInfo.CurrentQuantity > Quantity)
	{
		ItemComponent->ItemInfo.CurrentQuantity -= Quantity;
	}
	else
	{
		FiltertedActor->Destroy();
	}
	OutNote = FText::FromString("성공적으로 아이템을 줄임.");
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
		UE_LOG(LogTemp, Warning, TEXT("Items Length  = %f"), Items.Num());

		UPeCoItemComponent* ItemComponent = UPeCoFunctionLibrary::GetItemComponent(ItemActor);
		if (!IsValid(ItemComponent))
		{
			continue;
		}

		if (!ItemComponent->ItemInfo.ItemTag.MatchesTag(PeCoGameplayTags::Item))
		{
			Items.RemoveAt(i);
			continue;
		}
	}
	return Items;
}

bool UInventoryComponent::GetItemOfClass(const TSubclassOf<AActor> Class, AActor*& OutActor)
{
	TArray<AActor*> TargetArray = GetAllItems();

	for (auto It = TargetArray.CreateConstIterator(); It; It++)
	{
		AActor* TargetElement = (*It);
		if (TargetElement && TargetElement->IsA(Class))
		{
			OutActor = TargetElement;
			return true;
		}
	}
	return false;
}
bool UInventoryComponent::GetItemOfTag(const FGameplayTag ItemTag, AActor*& OutActor)
{
	TArray<AActor*> TargetArray = GetAllItems();

	for (auto It = TargetArray.CreateConstIterator(); It; It++)
	{
		AActor* TargetElement = (*It);
		if (TargetElement)
		{
			UPeCoItemComponent* ItemComponent = UPeCoFunctionLibrary::GetItemComponent(TargetElement);
			if (IsValid(ItemComponent) && ItemComponent->ItemInfo.ItemTag.MatchesTagExact(ItemTag))
			{
				OutActor = TargetElement;
				return true;
			}
		}
	}
	return false;
}
bool UInventoryComponent::GetAlItemsOfTag(const FGameplayTag ItemTag, TArray<AActor*>& OutFilteredArray)
{
	TArray<AActor*> FilteredArray;
	FilteredArray.Empty();

	TArray<AActor*> AllItmes = GetAllItems();

	for (auto It = AllItmes.CreateConstIterator(); It; It++)
	{
		AActor* TargetElement = (*It);
		if (TargetElement)
		{
			UPeCoItemComponent* ItemComponent = UPeCoFunctionLibrary::GetItemComponent(TargetElement);
			if (IsValid(ItemComponent) && ItemComponent->ItemInfo.ItemTag.MatchesTag(ItemTag))
			{
				FilteredArray.Add(TargetElement);
			}
		}
	}
	if (FilteredArray.Num() > 0)
	{
		OutFilteredArray = FilteredArray;
		return true;
	}

	return false;
}



bool UInventoryComponent::HasEnoughItems(const TSubclassOf<AActor> Item, const int32 Quantity, UPARAM(DisplayName = "Note") FText& OutNote)
{
	if (Quantity <= 0)
	{
		OutNote = FText::FromString("Quantity는 반드시 0보다 커야함.");
		return false;
	}

	int32 QuantityMissing = Quantity;

	AActor* FilteredActor;
	if (!GetItemOfClass(Item, FilteredActor))
	{
		OutNote = FText::FromString("Has enough 확인 실패: 해당 Class로 아이템을 찾을 수 없음.");
		return false;
	}

	const UPeCoItemComponent* ItemComponent = UPeCoFunctionLibrary::GetItemComponent(FilteredActor);
	if (!ensure(IsValid(ItemComponent)))
	{
		OutNote = FText::FromString("해당 Item에 ItemComponent가 존재하지 않음.");
		return false;
	}

	if (QuantityMissing <= ItemComponent->ItemInfo.CurrentQuantity)
	{
		OutNote = FText::FromString("성공. 충분한 아이템을 가지고 있음.");
		return true;
	}
	OutNote = FText::FromString("실패. 아이템이 부족함.");
	return false;
}
int32 UInventoryComponent::GetQuantityOfItem(const TSubclassOf<AActor> Class)
{
	int32 result = 0;
	AActor* FilteredActor;
	if (GetItemOfClass(Class, FilteredActor))
	{
		UPeCoItemComponent* ItemComponent = UPeCoFunctionLibrary::GetItemComponent(FilteredActor);
		if (!ensure(IsValid(ItemComponent)))
		{
			return 0;
		}
		result += ItemComponent->ItemInfo.CurrentQuantity;
	}
	return result;
 }

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



 void UInventoryComponent::AddPlayerMoney(const int32 Amount, FText& OutNote)
 {
	 PlayerMoney += Amount;

 }

 bool UInventoryComponent::HasEnoughMoney(const int32 Quantity, FText& OutNote)
 {
	 if (Quantity <= 0)
	 {
		 OutNote = FText::FromString("Quantity는 반드시 0보다 커야함.");
		 return false;
	 }
	 return PlayerMoney >= Quantity ? true : false;
 }
