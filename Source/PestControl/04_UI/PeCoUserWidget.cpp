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

#include "PeCoGameplayTags.h"
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

FText UPeCoUserWidget::ConstructStatTextFromTag(const FGameplayTag& GameplayTag, float Value)
{
	FGameplayTagContainer TimeRelavantTags = FGameplayTagContainer();
	TimeRelavantTags.AddTag(PeCoGameplayTags::WeaponStat_AttackSpeed); 	
	TimeRelavantTags.AddTag(PeCoGameplayTags::PlayerStat_SkillCoolTime);
	if (GameplayTag.MatchesAnyExact(TimeRelavantTags))
	{
		return FText::FromString(FString::Printf(TEXT("%.1f s"), Value)); 
	}
	else if (GameplayTag.MatchesTagExact(PeCoGameplayTags::PlayerStat_MoveSpeed))
	{
		return FText::FromString(FString::Printf(TEXT("%.1f cm/s"), Value)); 
	}
	else if (GameplayTag.MatchesTagExact(PeCoGameplayTags::WeaponStat_CriticalChance))
	{
		return FText::FromString(FString::Printf(TEXT("%.1f%%"), Value));
	}
	else if (GameplayTag.MatchesTag(PeCoGameplayTags::Item))
	{
		return FText::FromString(FString::Printf(TEXT("%1d"), Value));
	}

	return FText::FromString(FString::Printf(TEXT("%.1f"), Value)); 
}


FText UPeCoUserWidget::ConstructRewardTextFromTag(const FGameplayTag& GameplayTag, float Value)
{

	if (GameplayTag.MatchesTagExact(PeCoGameplayTags::WeaponStat_Damage) || GameplayTag.MatchesTagExact(PeCoGameplayTags::PlayerStat_MaxHealth))
	{
		return FText::FromString(FString::Printf(TEXT("%.1f"), Value)); 
	}
	else if (GameplayTag.MatchesTagExact(PeCoGameplayTags::PlayerStat_SkillCoolTime))
	{
		return FText::FromString(FString::Printf(TEXT("%.1f s"), Value));
	}

	return FText::FromString(FString::Printf(TEXT("%.1f%%"), Value));
}
