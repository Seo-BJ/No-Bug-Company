// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PeCoGameplayTags.h"

#include "StoreComponent.generated.h"

USTRUCT(BlueprintType)
struct FItemData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	FGameplayTag ItemTag;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	TSubclassOf<AActor> ItemClass;
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PESTCONTROL_API UStoreComponent : public UActorComponent
{
	GENERATED_BODY()

public:	

	UStoreComponent();

protected:

	virtual void BeginPlay() override;

public:	


	UFUNCTION(BlueprintCallable)
	FGameplayTagContainer GetRandomStatTags(int32 count);

	UFUNCTION(BlueprintCallable)
	float  GetStatUpgradeData(FGameplayTag StatTag, APlayerController* PlayerController);

	UFUNCTION(BlueprintCallable, Category = "InventorySystem|Inventory")
	bool BuyItemByTag(FGameplayTag ItemTag, FText& OutNote);

	UFUNCTION(BlueprintCallable, Category = "InventorySystem|Inventory")
	bool SellItemByTag(FGameplayTag ItemTag, FText& OutNote);
};
