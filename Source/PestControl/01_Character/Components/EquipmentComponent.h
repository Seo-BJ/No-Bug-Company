// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "21_Data/PeCoDataTypes.h"

#include "EquipmentComponent.generated.h"


USTRUCT(BlueprintType)
struct FEquipmentInfo
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "InventorySystem|Equipment")
	ESlotType CurrentSlot;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "InventorySystem|Equipment")
	ESlotType AcceptableSlotType;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "InventorySystem|Equipment")
	AActor* ItemActor = nullptr;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnItemEquip, AActor*, Item, ESlotType, Slot);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnItemUnEquip, AActor*, Item, ESlotType, Slot);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnItemUsedInSlot, AActor*, Item, ESlotType, Slot);

class UPeCoItemComponent;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PESTCONTROL_API UEquipmentComponent : public UActorComponent
{
	GENERATED_BODY()

	friend UPeCoItemComponent;

public:	

	UEquipmentComponent();

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "InventorySystem|Equipment")
	TArray<FEquipmentInfo> EquipmentList;

	UFUNCTION(BlueprintCallable, Category = "InventorySystem|Equipment")
	UPARAM(DisplayName = "Success") bool UseItemInSlot(const ESlotType SlotType);

	UFUNCTION(BlueprintCallable, Category = "InventorySystem|Equipment")
	UPARAM(DisplayName = "Success") bool EquipItemInSlot(const ESlotType SlotType, AActor* ItemActor, UPARAM(DisplayName = "PreviousItem") AActor*& OutPreviousItem, UPARAM(DisplayName = "NewItem") AActor*& OutNewItem);

	UFUNCTION(BlueprintCallable, Category = "InventorySystem|Equipment")
	UPARAM(DisplayName = "Success") bool UnEquipItemFromSlot(const ESlotType SlotType,UPARAM(DisplayName = "ItemUnequipped") AActor*& OutItemUnequipped);

	UFUNCTION(BlueprintCallable, Category = "InventorySystem|Equipment")
	UPARAM(DisplayName = "Success") bool GetItemInSlot(const ESlotType SlotType, UPARAM(DisplayName = "Item") AActor*& OutItem);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "InventorySystem|Equipment")
	UPARAM(DisplayName = "Has Items") bool GetAllItems(UPARAM(DisplayName = "Items") TArray<AActor*>& OutItems);

	// 아이템 장착시 Broadcast
	UPROPERTY(BlueprintAssignable, Category = "InventorySystem|Events")
	FOnItemEquip OnItemEquip;

	// 아이템 장착 해제시 Broadcast
	UPROPERTY(BlueprintAssignable, Category = "InventorySystem|Events")
	FOnItemUnEquip OnItemUnEquip;

	// Slot에 장착된 아이템을 슬롯에서 사용시 BroadCast
	UPROPERTY(BlueprintAssignable, Category = "InventorySystem|Events")
	FOnItemUnEquip OnItemUsedInSlot;



protected:

	virtual void BeginPlay() override;

private:








		
};
