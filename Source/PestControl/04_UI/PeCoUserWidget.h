// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PeCoUserWidget.generated.h"


class UPlayerStatPresenterComponent;
class UStoreComponent;
class UInventoryComponent;
class UEquipmentComponent;
/**
 * 
 */
UCLASS()
class PESTCONTROL_API UPeCoUserWidget : public UUserWidget
{
	GENERATED_BODY()
	


public:

	UFUNCTION(BlueprintCallable, BlueprintPure)
	UPlayerStatPresenterComponent* GetStatPresenter();	

	UFUNCTION(BlueprintCallable, BlueprintPure)
	UStoreComponent* GetStore();

	UFUNCTION(BlueprintCallable, BlueprintPure)
	UInventoryComponent* GetInventory();

	UFUNCTION(BlueprintCallable, BlueprintPure)
	UEquipmentComponent* GetEquipment();

	UFUNCTION(BlueprintCallable)
	FText ConstructStatTextFromTag(const FGameplayTag& GameplayTag, float Value);
	UFUNCTION(BlueprintCallable)
	FText ConstructRewardTextFromTag(const FGameplayTag& GameplayTag, float Value);
};
