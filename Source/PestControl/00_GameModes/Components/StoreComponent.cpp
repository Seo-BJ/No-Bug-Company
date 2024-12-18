// Fill out your copyright notice in the Description page of Project Settings.


#include "StoreComponent.h"

#include "00_GameModes/PeCoGameMode.h"

#include "01_Character/PeCoPlayerCharacter.h"
#include "01_Character/Components/InventoryComponent.h"
#include "01_Character/Components/EquipmentComponent.h"

#include "02_Player/PeCoPlayerState.h"
#include "02_Player/PlayerStats.h"
#include "02_Player/PeCoPlayerController.h"

#include "07_Weapon/Weapon.h"
#include "09_Items/Components/PeCoItemComponent.h"
#include "20_System/PeCoGameInstance.h"
#include "20_System/PeCoFunctionLibrary.h"
#include "21_Data/PeCoDataRow.h"

#include "Kismet/GameplayStatics.h"
#include "Engine/CurveTable.h"
#include "Engine/DataTable.h"
#include "Engine/StreamableManager.h"
#include "Engine/AssetManager.h"


UStoreComponent::UStoreComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}


// Called when the game starts
void UStoreComponent::BeginPlay()
{
	Super::BeginPlay();

	
}


FGameplayTagContainer UStoreComponent::GetRandomRewardTags(int32 Count, APlayerController* PlayerController)
{
	if (!IsValid(PlayerController))
	{
		return FGameplayTagContainer();
	}
	APeCoPlayerState* PlayerState = PlayerController->GetPlayerState<APeCoPlayerState>();
	if (!IsValid(PlayerState))
	{
		return FGameplayTagContainer();
	}
	APeCoPlayerCharacter* PlayerCharacter = PlayerState->GetPawn<APeCoPlayerCharacter>();
	if (!IsValid(PlayerCharacter))
	{
		return FGameplayTagContainer();
	}
	AWeapon* Weapon = PlayerCharacter->PlayerWeapon;
	if (!IsValid(Weapon))
	{
		return FGameplayTagContainer();
	}

	FGameplayTagContainer TagContainer = PeCoGameplayTags::GetChildTags(PeCoGameplayTags::PlayerStat);
	TagContainer.AppendTags(PeCoGameplayTags::GetChildTags(PeCoGameplayTags::Item_Combat));
	TagContainer.AppendTags(PeCoGameplayTags::GetChildTags(PeCoGameplayTags::Item_Consumption));
	TagContainer.AppendTags(PeCoGameplayTags::GetChildTags(PeCoGameplayTags::WeaponStat));
	TagContainer.RemoveTag(PeCoGameplayTags::PlayerStat_Health);
	TagContainer.RemoveTag(PeCoGameplayTags::WeaponStat_MaxAmmo);

	FGameplayTagContainer RemoveContainer = FGameplayTagContainer();
	for (const FGameplayTag& Tag : TagContainer)
	{
		if (Tag.MatchesTag(PeCoGameplayTags::PlayerStat))
		{
			if ((PlayerState->GetStatByTag(Tag)).GetStatLevel() >= 5)
			{
				RemoveContainer.AddTag(Tag);
			}
		}
		else if (Tag.MatchesTag(PeCoGameplayTags::WeaponStat))
		{
			int32* StatLevel = Weapon->WeaponStatLevelMap.Find(Tag);
			if (*StatLevel >= 5)
			{
				RemoveContainer.AddTag(Tag);
			}
		}
	}
	TagContainer.RemoveTags(RemoveContainer);

	FGameplayTagContainer RandomTags = PeCoGameplayTags::GetRandomTags(TagContainer, Count);
	return RandomTags;
}

	


FGameplayTagContainer UStoreComponent::GetRandomStatTags(int32 Count, APlayerController* PlayerController)
{
	if (!IsValid(PlayerController))
	{
		return FGameplayTagContainer();
	}
	APeCoPlayerState* PlayerState = PlayerController->GetPlayerState<APeCoPlayerState>();
	if (!IsValid(PlayerState))
	{
		return FGameplayTagContainer();
	}
	APeCoPlayerCharacter* PlayerCharacter = PlayerState->GetPawn<APeCoPlayerCharacter>();
	if (!IsValid(PlayerCharacter))
	{
		return FGameplayTagContainer();
	}
	AWeapon* Weapon = PlayerCharacter->PlayerWeapon;
	if (!IsValid(Weapon))
	{
		return FGameplayTagContainer();
	}

	FGameplayTagContainer TagContainer = PeCoGameplayTags::GetChildTags(PeCoGameplayTags::PlayerStat);
	TagContainer.AppendTags(PeCoGameplayTags::GetChildTags(PeCoGameplayTags::WeaponStat));
	TagContainer.AppendTags(PeCoGameplayTags::GetChildTags(PeCoGameplayTags::WeaponStat));
	TagContainer.RemoveTag(PeCoGameplayTags::PlayerStat_Health);
	TagContainer.RemoveTag(PeCoGameplayTags::WeaponStat_MaxAmmo);

	FGameplayTagContainer RemoveContainer = FGameplayTagContainer();
	for (const FGameplayTag& Tag : TagContainer)
	{
		if (Tag.MatchesTag(PeCoGameplayTags::PlayerStat))
		{
			if ((PlayerState->GetStatByTag(Tag)).GetStatLevel() >= 5)
			{
				RemoveContainer.AddTag(Tag);
			}
		}
		else if (Tag.MatchesTag(PeCoGameplayTags::WeaponStat))
		{
			int32* StatLevel = Weapon->WeaponStatLevelMap.Find(Tag);
			if (*StatLevel >= 5)
			{
				RemoveContainer.AddTag(Tag);
			}
		}
	}
	TagContainer.RemoveTags(RemoveContainer);

	FGameplayTagContainer RandomTags = PeCoGameplayTags::GetRandomTags(TagContainer, Count);
	return RandomTags;
}

int32 UStoreComponent::GetPriceByRewardTagAndRarity(FGameplayTag RewardTag, ERewardRarity Rarity)
{
	APeCoGameMode* GameMode = CastChecked<APeCoGameMode>(GetOwner());
	UPeCoGameInstance* GameInstance = CastChecked<UPeCoGameInstance>(GameMode->GetGameInstance());
	UDataTable* StoreDataTable = GameInstance->StoreDataTable;
	if (!StoreDataTable)
	{
		UE_LOG(LogTemp, Error, TEXT("상점 데이터 테이블 없음."));
		return -1;
	}
	FStoreRewardPriceData* Row = StoreDataTable->FindRow<FStoreRewardPriceData>(RewardTag.GetTagName(), TEXT("Read Reward Price"), false);
	switch (Rarity)
	{
	case ERewardRarity::Common:
		return Row->CommonPrice;
		break;
	case ERewardRarity::Rare:
		return Row->RarePrice;
		break;
	case ERewardRarity::Epic:
		return Row->EpicPrice;
		break;
	case ERewardRarity::Legendary:
		return Row->LegendaryPrice;
		break;
	default:
		return -1;
		break;
	}
	return -1;
}
float UStoreComponent::GetStatUpgradeData(FGameplayTag StatTag, APlayerController* PlayerController)
{
	if (!IsValid(PlayerController))
	{
		return -1;
	}
	APeCoPlayerState* PlayerState = PlayerController->GetPlayerState<APeCoPlayerState>();
	if (!IsValid(PlayerState))
	{
		return -1;
	}
	APeCoPlayerCharacter* PlayerCharacter = PlayerState->GetPawn<APeCoPlayerCharacter>();
	if (!IsValid(PlayerCharacter))
	{
		return -1;
	}

	APeCoGameMode* GameMode = CastChecked<APeCoGameMode>(GetOwner());
	UPeCoGameInstance* GameInstance = CastChecked<UPeCoGameInstance>(GameMode->GetGameInstance());
	UCurveTable* UpgradeCurveTable = GameInstance->StatUpgradeCurveTable;
	if (!UpgradeCurveTable)
	{
		UE_LOG(LogTemp, Error, TEXT("플레이어 스탯 데이터 테이블 없음."));
		return -1;
	}
	int32 OriginStatLevel = 0;
	if (StatTag.MatchesTag(PeCoGameplayTags::PlayerStat))
	{
		FPeCoStatData TargetStat = PlayerState->GetStatByTag(StatTag);
		FName RowName = StatTag.GetTagName();
		FRealCurve* StatCurve = UpgradeCurveTable->FindCurve(RowName, TEXT(""));
		if (!StatCurve)
		{
			UE_LOG(LogTemp, Error, TEXT("커브 테이블에서 해당 Stat Curve를 찾지 못함!"));
			return -1;
		}
		OriginStatLevel = TargetStat.GetStatLevel();
		int32 TargetStatLevel = OriginStatLevel + 1;
		return StatCurve->Eval(TargetStatLevel);
	}
	else if (StatTag.MatchesTag(PeCoGameplayTags::WeaponStat))
	{
		FName RowName = StatTag.GetTagName();
		FRealCurve* StatCurve = UpgradeCurveTable->FindCurve(RowName, TEXT(""));
		if (!StatCurve)
		{
			UE_LOG(LogTemp, Error, TEXT("커브 테이블에서 해당 Stat Curve를 찾지 못함!"));
			return -1;
		}
		OriginStatLevel = *PlayerCharacter->PlayerWeapon->WeaponStatLevelMap.Find(StatTag);
		int32 TargetStatLevel = OriginStatLevel + 1;
		return StatCurve->Eval(TargetStatLevel);
		 
	}
	return -1;
}

bool UStoreComponent::BuyItemByTag(FGameplayTag ItemTag, FText& OutNote, AController* User)
{
	if (!ItemClassMap.Contains(ItemTag))
	{
		UE_LOG(LogTemp, Warning, TEXT("Item with tag %s not found in shop!"), *ItemTag.ToString());
		return false;
	}
	APawn* Pawn = User->GetPawn();
	if (!IsValid(Pawn))
	{
		return false;
	}
	UInventoryComponent* InventoryComponent = UPeCoFunctionLibrary::GetInventoryComponent(Pawn);
	if (!IsValid(InventoryComponent))
	{
		return false;
	}
	bool bCanBuyItem = InventoryComponent->HasEnoughMoney(MaterialPurchasePrice, OutNote);
	if (!bCanBuyItem)
	{
		return false;
	}
	// 돈이 있으면 소프트 오브젝트 포인터에서 클래스 로드
	TSoftClassPtr<AActor> SoftItemClass = ItemClassMap[ItemTag];
	if (IsValid(SoftItemClass.Get()))
	{
		OnItemClassLoaded(ItemTag, User);
		return true;
	}
	else
	{

		FStreamableManager& Streamable = UAssetManager::GetStreamableManager();
		Streamable.RequestAsyncLoad(SoftItemClass.ToSoftObjectPath(), FStreamableDelegate::CreateUObject(this, &UStoreComponent::OnItemClassLoaded, ItemTag, User));
		return true;
	}
}
bool UStoreComponent::SellItemByTag(FGameplayTag ItemTag, FText& OutNote, AController* User)
{
	if (!IsValid(User))
	{
		return false;
	}
	APawn* Pawn = User->GetPawn();
	if (!IsValid(Pawn))
	{
		return false;
	}
	UInventoryComponent* InventoryComponent = UPeCoFunctionLibrary::GetInventoryComponent(Pawn);
	if (!IsValid(InventoryComponent))
	{
		return false;
	}
	return InventoryComponent->SellItemInternal(ItemTag, MaterialSellingPrice);
}
void UStoreComponent::OnItemClassLoaded(FGameplayTag ItemTag, AController* User)
{
	if (!ItemClassMap.Contains(ItemTag)) return;
	TSoftClassPtr<AActor> SoftItemClass = ItemClassMap[ItemTag];

	if (!IsValid(SoftItemClass.Get()))
	{
		return;
	}
	APawn* Pawn = Cast<APeCoPlayerController>(User)->GetPawn();
	if (!IsValid(Pawn))
	{
		return;
	}
	UInventoryComponent* InventoryComponent = UPeCoFunctionLibrary::GetInventoryComponent(Pawn);
	if (!IsValid(InventoryComponent))
	{
		return;
	}
	InventoryComponent->BuyItemInternal(SoftItemClass.Get(), MaterialPurchasePrice);
}