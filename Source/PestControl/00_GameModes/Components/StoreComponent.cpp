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
	FGameplayTagContainer TagContainer = PeCoGameplayTags::GetChildTags(PeCoGameplayTags::PlayerStat);
	TagContainer.AppendTags(PeCoGameplayTags::GetChildTags(PeCoGameplayTags::Item_Combat));
	TagContainer.AppendTags(PeCoGameplayTags::GetChildTags(PeCoGameplayTags::Item_Consumption));
	TagContainer.AppendTags(PeCoGameplayTags::GetChildTags(PeCoGameplayTags::WeaponStat));
	TagContainer.RemoveTag(PeCoGameplayTags::PlayerStat_Health);
	TagContainer.RemoveTag(PeCoGameplayTags::WeaponStat_MaxAmmo);

	return PickRandomUpgradableTags(TagContainer, Count, PlayerController);
}

FGameplayTagContainer UStoreComponent::GetRandomStatTags(int32 Count, APlayerController* PlayerController)
{
	FGameplayTagContainer TagContainer = PeCoGameplayTags::GetChildTags(PeCoGameplayTags::PlayerStat);
	TagContainer.AppendTags(PeCoGameplayTags::GetChildTags(PeCoGameplayTags::WeaponStat));
	TagContainer.RemoveTag(PeCoGameplayTags::PlayerStat_Health);
	TagContainer.RemoveTag(PeCoGameplayTags::WeaponStat_MaxAmmo);

	return PickRandomUpgradableTags(TagContainer, Count, PlayerController);
}

FGameplayTagContainer UStoreComponent::PickRandomUpgradableTags(FGameplayTagContainer TagContainer, int32 Count, APlayerController* PlayerController)
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
			const int32* StatLevel = Weapon->WeaponStatLevelMap.Find(Tag);
			if (StatLevel && *StatLevel >= 5)
			{
				RemoveContainer.AddTag(Tag);
			}
		}
	}
	TagContainer.RemoveTags(RemoveContainer);

	return PeCoGameplayTags::GetRandomTags(TagContainer, Count);
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
	if (!Row)
	{
		UE_LOG(LogTemp, Error, TEXT("StoreDataTable에서 %s에 해당하는 Row를 찾지 못함."), *RewardTag.ToString());
		return -1;
	}
	switch (Rarity)
	{
	case ERewardRarity::Common:
		return Row->CommonPrice;
	case ERewardRarity::Rare:
		return Row->RarePrice;
	case ERewardRarity::Epic:
		return Row->EpicPrice;
	case ERewardRarity::Legendary:
		return Row->LegendaryPrice;
	default:
		return -1;
	}
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
		const int32* StatLevelPtr = PlayerCharacter->PlayerWeapon->WeaponStatLevelMap.Find(StatTag);
		if (!StatLevelPtr)
		{
			UE_LOG(LogTemp, Error, TEXT("WeaponStatLevelMap에서 %s를 찾지 못함."), *StatTag.ToString());
			return -1;
		}
		OriginStatLevel = *StatLevelPtr;
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
	const int32 Price = GetPurchasePriceForItem(ItemTag);
	if (Price < 0)
	{
		return false;
	}
	if (!InventoryComponent->HasEnoughMoney(Price, OutNote))
	{
		return false;
	}
	// 비동기 로드 요청
	TSoftClassPtr<AActor>& SoftItemClass = ItemClassMap[ItemTag];
	TWeakObjectPtr<AController> WeakUser = User;
	if (IsValid(SoftItemClass.Get()))
	{
		OnItemClassLoaded(ItemTag, WeakUser);
		return true;
	}
	else
	{
		// 같은 아이템에 대한 이전 요청이 남아있다면 취소 후 재요청
		if (TSharedPtr<FStreamableHandle>* ExistingHandle = PendingBuyHandles.Find(ItemTag))
		{
			if (ExistingHandle->IsValid())
			{
				(*ExistingHandle)->CancelHandle();
			}
			PendingBuyHandles.Remove(ItemTag);
		}

		FStreamableManager& Streamable = UAssetManager::GetStreamableManager();
		TSharedPtr<FStreamableHandle> Handle = Streamable.RequestAsyncLoad(
			SoftItemClass.ToSoftObjectPath(),
			FStreamableDelegate::CreateUObject(this, &UStoreComponent::OnItemClassLoaded, ItemTag, WeakUser));
		if (Handle.IsValid())
		{
			PendingBuyHandles.Add(ItemTag, Handle);
		}
		return true;
	}
}

void UStoreComponent::OnItemClassLoaded(FGameplayTag ItemTag, TWeakObjectPtr<AController> User)
{
	// 완료된 핸들 정리
	PendingBuyHandles.Remove(ItemTag);

	if (!ItemClassMap.Contains(ItemTag)) return;
	TSoftClassPtr<AActor> SoftItemClass = ItemClassMap[ItemTag];

	if (!IsValid(SoftItemClass.Get())) return;

	APeCoPlayerController* PlayerController = Cast<APeCoPlayerController>(User.Get());
	if (!IsValid(PlayerController)) return;

	APawn* Pawn = PlayerController->GetPawn();
	if (!IsValid(Pawn)) return;

	UInventoryComponent* InventoryComponent = UPeCoFunctionLibrary::GetInventoryComponent(Pawn);
	if (!IsValid(InventoryComponent)) return;

	const int32 Price = GetPurchasePriceForItem(ItemTag);
	if (Price < 0)
	{
		return;
	}
	InventoryComponent->BuyItemInternal(SoftItemClass.Get(), Price);
}

int32 UStoreComponent::GetPurchasePriceForItem(FGameplayTag ItemTag) const
{
	if (ItemTag.MatchesTag(PeCoGameplayTags::Item_Material))
	{
		return MaterialPurchasePrice;
	}
	if (ItemTag.MatchesTag(PeCoGameplayTags::Item_Combat) || ItemTag.MatchesTag(PeCoGameplayTags::Item_Consumption))
	{
		return ItemPurchasePrice;
	}
	return -1;
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