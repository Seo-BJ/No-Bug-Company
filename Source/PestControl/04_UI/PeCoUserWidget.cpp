// Fill out your copyright notice in the Description page of Project Settings.


#include "PeCoUserWidget.h"
#include "00_GameModes/PeCoGameMode.h"
#include "00_GameModes/Components/StoreComponent.h"
#include "01_Character/Components/EquipmentComponent.h"
#include "01_Character/Components/InventoryComponent.h"
#include "02_Player/PeCoPlayerState.h"
#include "02_Player/Components/PlayerLevelPresenterComponent.h"
#include "02_Player/Components/PlayerStatPresenterComponent.h"
#include "20_System/PeCoFunctionLibrary.h"

#include "Kismet/GameplayStatics.h"

UPlayerStatPresenterComponent* UPeCoUserWidget::GetStatPresenter()
{
	APeCoPlayerState* PlayerState = GetOwningPlayerState<APeCoPlayerState>();
	if (IsValid(PlayerState))
	{
		UPlayerStatPresenterComponent* StatPresenter = PlayerState->FindComponentByClass<UPlayerStatPresenterComponent>();
		if (IsValid(StatPresenter))
		{
			return StatPresenter;
		}
	}
	return nullptr;
}

UStoreComponent* UPeCoUserWidget::GetStore()
{
	AGameModeBase* GameMode = UGameplayStatics::GetGameMode(this);
	if (IsValid(GameMode))
	{
		APeCoGameMode* PeCoGameMode = CastChecked<APeCoGameMode>(GameMode);
		UStoreComponent* StoreComponent = PeCoGameMode->FindComponentByClass<UStoreComponent>();
		if (IsValid(StoreComponent))
		{
			return StoreComponent;
		}
	}
	return nullptr;
}

UInventoryComponent* UPeCoUserWidget::GetInventory()
{
	return UPeCoFunctionLibrary::GetInventoryComponent(GetOwningPlayer()->GetPawn());
}

UEquipmentComponent* UPeCoUserWidget::GetEquipment()
{
	return UPeCoFunctionLibrary::GetEquipmentComponent(GetOwningPlayer()->GetPawn());
}
