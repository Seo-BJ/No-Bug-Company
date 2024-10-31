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

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Equipment")
	FName Id;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Equipment")
	EItemType AcceptableSlotType;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Equipment")
	AActor* ItemActor = nullptr;
};

class APeCoItem;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PESTCONTROL_API UEquipmentComponent : public UActorComponent
{
	GENERATED_BODY()

	friend APeCoItem;

public:	

	UEquipmentComponent();

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Equipment")
	TArray<FEquipmentInfo> EquipmentList;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "AGR")
	UPARAM(DisplayName = "Has Items") bool GetAllItems(UPARAM(DisplayName = "Items") TArray<AActor*>& OutItems);

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "AGR")
	UPARAM(DisplayName = "Success") bool EquipItemInSlot(const FName Slot, AActor* ItemActor, UPARAM(DisplayName = "PreviousItem") AActor*& OutPreviousItem, UPARAM(DisplayName = "NewItem") AActor*& OutNewItem);

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "AGR")
	UPARAM(DisplayName = "Success") bool UnEquipItemFromSlot(const FName Slot,UPARAM(DisplayName = "ItemUnequipped") AActor*& OutItemUnequipped);

	UFUNCTION(BlueprintCallable, Category = "AGR")
	UPARAM(DisplayName = "Success") bool GetItemInSlot(const FName Slot, UPARAM(DisplayName = "Item") AActor*& OutItem);

protected:

	virtual void BeginPlay() override;




public:	
	




		
};
