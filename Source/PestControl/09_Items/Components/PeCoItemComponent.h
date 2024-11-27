// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PeCoGameplayTags.h"
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
	FGameplayTag ItemTag = FGameplayTag();

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "InventorySystem|Item|Information")
	FString Name;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "InventorySystem|Item|Information")
	FString Description;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "InventorySystem|Item|Information")
	bool bSimulateWhenDropped = false;
};

class UInventoryComponent;
class UEquipmentComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnHiddenShown, bool, bHidden);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnItemDropped);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnItemUsed, AActor*, User);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnEquip, AActor*, User, FGameplayTag, SlotTag);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnUnEquip, AActor*, User, FGameplayTag, SlotTag);

UCLASS(BlueprintType, Blueprintable, ClassGroup = ("InventorySystem"), meta = (BlueprintSpawnableComponent))
class PESTCONTROL_API UPeCoItemComponent : public UActorComponent
{
	GENERATED_BODY()

	friend UInventoryComponent;
	friend UEquipmentComponent;

public:	

	UPeCoItemComponent();

	virtual bool UseItem(AActor* User, bool bNeedSpawn);

	UFUNCTION(BlueprintCallable, Category = "InventorySystem|Item")
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

	bool SpawnItemForUse(AActor*& OutSpawnActor);
	void HideShowItem(const bool bHide);
	void EquipInternal(const FGameplayTag SlotTag) const;
	void UnEquipInternal(const FGameplayTag SlotTag) const;

		
};

