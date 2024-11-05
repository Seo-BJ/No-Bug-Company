// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "21_Data/PeCoDataTypes.h"

#include "InventoryComponent.generated.h"


class APeCoPlayerController;
class UPeCoItemComponent;


DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnItemUpdated, AActor*, Item);

UCLASS(BlueprintType, Blueprintable, ClassGroup = ("InventorySystem"), meta = (BlueprintSpawnableComponent))
class PESTCONTROL_API UInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

	friend UPeCoItemComponent;

public:	

	UInventoryComponent();

	//UPROPERTY(BlueprintReadWrite, EditAnywhere, Replicated, SaveGame, Category = "AGR|Game Play")
	//TArray<FEquipmentInfo> EquipmentList;

	UPROPERTY(BlueprintReadWrite, Category = "InventorySystem|Inventory")
	AActor* InventoryStorage = nullptr;

	UPROPERTY(BlueprintAssignable, Category = "InventorySystem|Events")
	FOnItemUpdated OnItemUpdated;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	

	UFUNCTION(BlueprintCallable, Category = "InventorySystem|Inventory")
	UPARAM(DisplayName = "Success") bool AddItemsOfClass(const TSubclassOf<AActor> Class, const int32 Quantity, FText& OutNote);

	UFUNCTION(BlueprintCallable, Category = "InventorySystem|Inventory")
	UPARAM(DisplayName = "Success") bool RemoveItemsOfClass(const TSubclassOf<AActor> Class, const int32 Quantity, FText& OutNote);


	UFUNCTION(BlueprintCallable, Category = "InventorySystem|Inventory")
	UPARAM(DisplayName = "Found") bool GetItemOfClass(const TSubclassOf<AActor> Class, UPARAM(DisplayName = "TargetActor") AActor*& OutActor);

	UFUNCTION(BlueprintCallable, Category = "InventorySystem|Inventory")
	UPARAM(DisplayName = "Success") bool GetAlItemsOfType(const EItemType ItemType, UPARAM(DisplayName = "FilteredArray") TArray<AActor*>& OutFilteredArray);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "InventorySystem|Inventory")
	UPARAM(DisplayName = "Items") TArray<AActor*> GetAllItems();



	UFUNCTION(BlueprintCallable, Category = "InventorySystem|Inventory")
	UPARAM(DisplayName = "Success") bool HasEnoughItems(const TSubclassOf<AActor> Item,const int32 Quantity, FText& OutNote);


	UFUNCTION(BlueprintCallable, Category = "InventorySystem|Inventory")
	UPARAM(DisplayName = "Quantity") int32 GetQuantityOfItem(const TSubclassOf<AActor> Class);


	/*
	* UFUNCTION(BlueprintCallable, BlueprintPure, Category = "InventorySystem|Equipment")
		UPARAM(DisplayName = "Success") bool GetNextIndexItem(UPARAM(DisplayName = "Items") TArray<AActor*>& OutItems);
	* 
	* 
		*/



	void SetupInventoryStorageReference();

private:

};
