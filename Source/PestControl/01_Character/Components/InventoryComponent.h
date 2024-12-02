// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "21_Data/PeCoDataTypes.h"

#include "InventoryComponent.generated.h"


class APeCoPlayerController;
class UPeCoItemComponent;


DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnItemUpdated, AActor*, Item);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FPlayerMoneyChanged, float, OldMoney, float, NewMoney);

UCLASS(BlueprintType, Blueprintable, ClassGroup = ("InventorySystem"), meta = (BlueprintSpawnableComponent))
class PESTCONTROL_API UInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

	friend UPeCoItemComponent;

public:	

	UInventoryComponent();


	UPROPERTY(BlueprintAssignable, Category = "InventorySystem|Events")
	FPlayerMoneyChanged OnPlayerMoneyChanged;

	UPROPERTY(BlueprintReadWrite, Category = "InventorySystem|Inventory")
	AActor* InventoryStorage = nullptr;

	UPROPERTY(BlueprintAssignable, Category = "InventorySystem|Events")
	FOnItemUpdated OnItemUpdated;

	UFUNCTION(BlueprintCallable, Category = "InventorySystem|Inventory")
	UPARAM(DisplayName = "Success") bool AddItemsOfClass(const TSubclassOf<AActor> Class, const int32 Quantity, FText& OutNote);

	UFUNCTION(BlueprintCallable, Category = "InventorySystem|Inventory")
	UPARAM(DisplayName = "Success") bool RemoveItemsOfClass(const TSubclassOf<AActor> Class, const int32 Quantity, FText& OutNote);

	UFUNCTION(BlueprintCallable, Category = "InventorySystem|Inventory")
	UPARAM(DisplayName = "Success") bool RemoveItemsOfTag(FGameplayTag ItemTag, const int32 Quantity, FText& OutNote);

	UFUNCTION(BlueprintCallable, Category = "InventorySystem|Inventory")
	UPARAM(DisplayName = "Found") bool GetItemOfClass(const TSubclassOf<AActor> Class, UPARAM(DisplayName = "TargetActor") AActor*& OutActor);

	// Find Item By Tag
	UFUNCTION(BlueprintCallable, Category = "InventorySystem|Inventory")
	UPARAM(DisplayName = "Found") bool GetItemOfTag(const FGameplayTag ItemTag, UPARAM(DisplayName = "TargetActor") AActor*& OutActor);

	UFUNCTION(BlueprintCallable, Category = "InventorySystem|Inventory")
	UPARAM(DisplayName = "Found") bool GetAlItemsOfTag(const FGameplayTag ItemTag, UPARAM(DisplayName = "FilteredArray") TArray<AActor*>& OutFilteredArray);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "InventorySystem|Inventory")
	UPARAM(DisplayName = "Items") TArray<AActor*> GetAllItems();

	UFUNCTION(BlueprintCallable, Category = "InventorySystem|Inventory")
	UPARAM(DisplayName = "Success") bool HasEnoughItems(const TSubclassOf<AActor> Item, const int32 Quantity, FText& OutNote);

	UFUNCTION(BlueprintCallable, Category = "InventorySystem|Inventory")
	UPARAM(DisplayName = "Quantity") int32 GetQuantityOfItem(const TSubclassOf<AActor> Class);

	void SetupInventoryStorageReference();

	UFUNCTION(BlueprintCallable, Category = "InventorySystem|Inventory")
	void AddPlayerMoney(const int32 Amount, FText& OutNote);
	UFUNCTION(BlueprintCallable, Category = "InventorySystem|Inventory")
	bool HasEnoughMoney(const int32 Quantity, FText& OutNote);

	void BuyItemInternal(const TSubclassOf<AActor> Class, int32 PurchasePrice);
	void SellItemInternal(FGameplayTag ItemTag, int32 SellingPrice);

protected:
	virtual void BeginPlay() override;

private:

	UPROPERTY(VisibleDefaultsOnly, Category = "InventorySystem|Inventory")
	int PlayerMoney = 0;

public:

	UFUNCTION(BlueprintCallable, BlueprintPure)
	int GetPlayerMoney() { return PlayerMoney; }


};
