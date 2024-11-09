// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "01_Character/Components/EquipmentComponent.h"
#include "01_Character/Components/InventoryComponent.h"
#include "01_Character/Components/BuffComponent.h"
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

    FORCEINLINE static UBuffComponent* GetBuffComponent(const AActor* Actor)
    {
        return IsValid(Actor)
            ? Cast<UBuffComponent>(Actor->GetComponentByClass(UBuffComponent::StaticClass()))
            : nullptr;
    }


private:
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "InventorySystem")
    static UPeCoItemComponent* K2_GetItemComponent(AActor* Actor)
    {
        return GetItemComponent(Actor);
    };

    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "InventorySystem")
    static UInventoryComponent* K2_GetInventoryComponent(AActor* Actor)
    {
        return GetInventoryComponent(Actor);
    };

    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "InventorySystem")
    static UEquipmentComponent* K2_GetEquipmentComponent(AActor* Actor)
    {
        return GetEquipmentComponent(Actor);
    };

    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "BuffSystem")
    static UBuffComponent* K2_GetBuffComponent(AActor* Actor)
    {
        return GetBuffComponent(Actor);
    };


};
