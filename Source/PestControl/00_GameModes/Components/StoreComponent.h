// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Engine/StreamableManager.h"
#include "PeCoGameplayTags.h"
#include "21_Data/PeCoDataTypes.h"

#include "StoreComponent.generated.h"

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent), Blueprintable )
class PESTCONTROL_API UStoreComponent : public UActorComponent
{
	GENERATED_BODY()

public:	

	UStoreComponent();

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	TMap<FGameplayTag, TSoftClassPtr<AActor>> ItemClassMap;


protected:

	virtual void BeginPlay() override;

public:	

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	int32 MaterialPurchasePrice = 40;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	int32 MaterialSellingPrice = 20;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	int32 ItemPurchasePrice = 100;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	int32 RewardPurchasePrice = 100;


	UFUNCTION(BlueprintCallable)
	FGameplayTagContainer GetRandomRewardTags(int32 Count, APlayerController* PlayerController);
	UFUNCTION(BlueprintCallable)
	FGameplayTagContainer GetRandomStatTags(int32 Count, APlayerController* PlayerController);

	UFUNCTION(BlueprintCallable)
	int32 GetPriceByRewardTagAndRarity(FGameplayTag RewardTag, ERewardRarity Rarity);

	UFUNCTION(BlueprintCallable)
	float  GetStatUpgradeData(FGameplayTag StatTag, APlayerController* PlayerController);

	UFUNCTION(BlueprintCallable, Category = "InventorySystem|Inventory")
	bool BuyItemByTag(FGameplayTag ItemTag, FText& OutNote, AController* User);

	UFUNCTION(BlueprintCallable, Category = "InventorySystem|Inventory")
	bool SellItemByTag(FGameplayTag ItemTag, FText& OutNote, AController* User);

private:

	void OnItemClassLoaded(FGameplayTag ItemTag, TWeakObjectPtr<AController> User);

	FGameplayTagContainer PickRandomUpgradableTags(FGameplayTagContainer TagContainer, int32 Count, APlayerController* PlayerController);

	int32 GetPurchasePriceForItem(FGameplayTag ItemTag) const;

	TMap<FGameplayTag, TSharedPtr<FStreamableHandle>> PendingBuyHandles;
};
