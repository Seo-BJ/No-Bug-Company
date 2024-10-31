// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "21_Data/PeCoDataTypes.h"

#include "PeCoItem.generated.h"




USTRUCT(BlueprintType)
struct FPeCoItemInfo
{

	GENERATED_BODY()

public:

	FPeCoItemInfo();

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "AGR|Quantity")
	bool bStackable = true;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "AGR|Quantity")
	int32 MaxStack = 10;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "AGR|Quantity")
	int32 CurrentStack = 1;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Data")
	EItemType ItemType = EItemType::EItemType_Default;


	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Data")
	TSubclassOf<APeCoItem> ItemClass = nullptr;


	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Data")
	UTexture2D* Image;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Data")
	FString Name;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Data")
	FString Description;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Data")
	int32 Quantity;
};

class UInventoryComponent;
class UEquipmentComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnHiddenShown, bool, bHidden);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnItemDropped);

// DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnItemUsed, AActor*, User, FGameplayTag, GameplayTag);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnEquip, AActor*, User, FName, SlotName);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnUnEquip, AActor*, User, FName, SlotName);




UCLASS()
class PESTCONTROL_API APeCoItem : public AActor
{
	GENERATED_BODY()

	friend UInventoryComponent;
	friend UEquipmentComponent;

public:
	// Sets default values for this actor's properties
	APeCoItem();

	static const FName TAG_ITEM;



	UFUNCTION(BlueprintCallable, Category = "Inventory|Item")
	void PickUpItem(UInventoryComponent* Inventory);

	UFUNCTION(BlueprintCallable, Category = "Inventory|Item")
	void UseItem();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventtory")
	FPeCoItemInfo ItemInfo;

	UPROPERTY(BlueprintAssignable, EditAnywhere, Category = "AGR|Events")
	FOnHiddenShown OnHiddenShown;

	UPROPERTY(BlueprintAssignable, EditAnywhere, Category = "AGR|Events")
	FOnEquip OnEquip;

	UPROPERTY(BlueprintAssignable, EditAnywhere, Category = "AGR|Events")
	FOnUnEquip OnUnEquip;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;




private:

	void HideShowItem(const bool bHide)  ;
	void EquipInternal(const FName Slot) ;
	void UnEquipInternal(const FName Slot) ;

};
