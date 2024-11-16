// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "01_Character/Components/EquipmentComponent.h"
#include "01_Character/Components/InventoryComponent.h"
#include "02_Player/Components/PlayerStatPresenterComponent.h"

#include "09_Items/Components/PeCoItemComponent.h"

#include "21_Data/PeCoDataTypes.h"

#include "PeCoFunctionLibrary.generated.h"

/**
 * 
 */
/*
class UInventoryComponent;
class UEquipmentComponent;
class UPeCoItemComponent;
*/
UCLASS()
class PESTCONTROL_API UPeCoFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:

    FORCEINLINE static UPeCoItemComponent* GetItemComponent(const AActor* Actor)
    {
        return IsValid(Actor)
            ? Cast<UPeCoItemComponent>(Actor->GetComponentByClass(UPeCoItemComponent::StaticClass()))
            : nullptr;
    }

    FORCEINLINE static UInventoryComponent* GetInventoryComponent(const AActor* Actor)
    {
        return IsValid(Actor)
            ? Cast<UInventoryComponent>(Actor->GetComponentByClass(UInventoryComponent::StaticClass()))
            : nullptr;
    }

    FORCEINLINE static UEquipmentComponent* GetEquipmentComponent(const AActor* Actor)
    {
        return IsValid(Actor)
            ? Cast<UEquipmentComponent>(Actor->GetComponentByClass(UEquipmentComponent::StaticClass()))
            : nullptr;
    }

    FORCEINLINE static UPlayerStatPresenterComponent* GetStatPresenter(const AActor* Actor)
    {
        return IsValid(Actor)
            ? Cast<UPlayerStatPresenterComponent>(Actor->GetComponentByClass(UPlayerStatPresenterComponent::StaticClass()))
            : nullptr;
    }



private:
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "InventorySystem", meta = (DisplayName = "GetItemComponent"))
    static UPeCoItemComponent* K2_GetItemComponent(AActor* Actor)
    {
        return GetItemComponent(Actor);
    };

    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "InventorySystem", meta = (DisplayName = "GetInventoryComponent"))
    static UInventoryComponent* K2_GetInventoryComponent(AActor* Actor)
    {
        return GetInventoryComponent(Actor);
    };

    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "InventorySystem", meta = (DisplayName = "GetEquipmentComponent"))
    static UEquipmentComponent* K2_GetEquipmentComponent(AActor* Actor)
    {
        return GetEquipmentComponent(Actor);
    };

    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "InventorySystem", meta = (DisplayName = "GetPlayerStatPresenterComponent"))
    static UPlayerStatPresenterComponent* K2_GetPlayerStatPresenterComponent(AActor* Actor)
    {
        return GetStatPresenter(Actor);
    };

};
