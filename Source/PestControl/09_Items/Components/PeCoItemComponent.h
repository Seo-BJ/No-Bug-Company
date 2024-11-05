// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "21_Data/PeCoDataTypes.h"

#include "PeCoItemComponent.generated.h"

USTRUCT(BlueprintType)
struct FPeCoItemInfo
{
	GENERATED_BODY()

public:

	FPeCoItemInfo();

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "InventorySystem|Information")
	bool bStackable = true;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "InventorySystem|Information")
	int32 CurrentQuantity = 0;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "InventorySystem|Information")
	EItemType ItemType = EItemType::EItemType_Default;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "InventorySystem|Information")
	EPlayerItem ItemEnum = EPlayerItem::EPlayerItem_Default;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "InventorySystem|Information")
	ESlotType SlotType = ESlotType::ESlotType_Default;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "InventorySystem|Item|Information")
	UTexture2D* Image;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "InventorySystem|Item|Information")
	FString Name;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "InventorySystem|Item|Information")
	FString Description;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "InventorySystem|Item|Events")
	int32 InventoryWheelSlotIndex = 0;

};

class UInventoryComponent;
class UEquipmentComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnHiddenShown, bool, bHidden);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnItemDropped);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnItemUsed, AActor*, User);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnEquip, AActor*, User, ESlotType, SlotType);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnUnEquip, AActor*, User, ESlotType, SlotType);

UCLASS(BlueprintType, Blueprintable, ClassGroup = ("InventorySystem"), meta = (BlueprintSpawnableComponent))
class PESTCONTROL_API UPeCoItemComponent : public UActorComponent
{
	GENERATED_BODY()

	friend UInventoryComponent;
	friend UEquipmentComponent;

public:	

	UPeCoItemComponent();

	static const FName TAG_ITEM;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Item")
	FName ItemName = TAG_ITEM;

	virtual bool UseItem(AActor* User);

	UFUNCTION(BlueprintCallable, Category = "Item")
	void PickUpItem(UInventoryComponent* Inventory);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "InventorySystem|Item")
	FPeCoItemInfo ItemInfo;

	UPROPERTY(BlueprintAssignable, EditAnywhere, Category = "InventorySystem|Item|Events")
	FOnHiddenShown OnHiddenShown;

	UPROPERTY(BlueprintAssignable, EditAnywhere, Category = "InventorySystem|Item|Events")
	FOnEquip OnEquip;

	UPROPERTY(BlueprintAssignable, EditAnywhere, Category = "InventorySystem|Item|Events")
	FOnUnEquip OnUnEquip;

	UPROPERTY(BlueprintAssignable, EditAnywhere, Category = "InventorySystem|Item|Events")
	FOnItemUsed OnItemUsed;

protected:

	virtual void BeginPlay() override;

private:

	void HideShowItem(const bool bHide) const;
	void EquipInternal(const ESlotType SlotType) const;
	void UnEquipInternal(const ESlotType SlotType) const;

		
};

