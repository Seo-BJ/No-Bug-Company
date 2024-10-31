// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "21_Data/PeCoDataTypes.h"

#include "InventoryComponent.generated.h"


class APeCoPlayerController;

class APotion;
class APeCoItem;
class ACombatItem;
class AConsumableItem;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnItemUpdated, AActor*, Item);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PESTCONTROL_API UInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

	friend APeCoItem;

public:	

	UInventoryComponent();

	//UPROPERTY(BlueprintReadWrite, EditAnywhere, Replicated, SaveGame, Category = "AGR|Game Play")
	//TArray<FEquipmentInfo> EquipmentList;


	UPROPERTY(BlueprintReadWrite, Category = "Inventory")
	AActor* InventoryStorage = nullptr;


	UPROPERTY(BlueprintAssignable, Category = "AGR|Events")
	FOnItemUpdated OnItemUpdated;



	UFUNCTION(BlueprintCallable, Category = "Inventory")
	UPARAM(DisplayName = "Success") bool EquipItemInSlot(const FName Slot, AActor* ItemActor, AActor*& OutPreviousItem, AActor*& OutNewItem);



protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	


	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Inventory")
	UPARAM(DisplayName = "Success") bool AddItemsOfClass(const TSubclassOf<AActor> Class, const int32 Quantity, FText& OutNote);

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "AGR")
	UPARAM(DisplayName = "Success") bool RemoveItemsOfClass(const TSubclassOf<AActor> Class, const int32 Quantity, FText& OutNote);


	UFUNCTION(BlueprintCallable, Category = "Inventory")
	UPARAM(DisplayName = "Found") bool GetAllItemsOfClass(const TSubclassOf<AActor> Class, UPARAM(DisplayName = "FilteredArray") TArray<AActor*>& OutFilteredArray);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Inventory")
	UPARAM(DisplayName = "Items") TArray<AActor*> GetAllItems();

	UFUNCTION(BlueprintCallable, Category = "AGR")
	UPARAM(DisplayName = "Success") bool HasEnoughItems(const TSubclassOf<AActor> Item,const int32 Quantity, FText& OutNote);


		
	void UseItemInQuickSlot(EItemType ItemType);

	APeCoItem* CycleItemSlot(EItemType ItemType, bool bIndexUp);

	void SetItemInQuickSlot(EItemType ItemType, APeCoItem* Item);





private:

	TObjectPtr<APeCoPlayerController> PlayerController;

	void SetupInventoryStorageReference();
		
};
