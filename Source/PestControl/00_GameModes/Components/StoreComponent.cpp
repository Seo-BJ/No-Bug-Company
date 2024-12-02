// Fill out your copyright notice in the Description page of Project Settings.


#include "StoreComponent.h"

#include "00_GameModes/PeCoGameMode.h"

#include "02_Player/PeCoPlayerState.h"
#include "02_Player/PlayerStats.h"

#include "20_System/PeCoGameInstance.h"
#include "21_Data/PeCoDataRow.h"

#include "Kismet/GameplayStatics.h"
#include "Engine/CurveTable.h"
#include "Engine/DataTable.h"


UStoreComponent::UStoreComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}


// Called when the game starts
void UStoreComponent::BeginPlay()
{
	Super::BeginPlay();

	
}


FGameplayTagContainer UStoreComponent::GetRandomStatTags(int32 count)
{
	FGameplayTagContainer TagContainer = PeCoGameplayTags::GetChildTags(PeCoGameplayTags::PlayerStat);
	TagContainer.RemoveTag(PeCoGameplayTags::PlayerStat_Health);
	FGameplayTagContainer RandomTags = PeCoGameplayTags::GetRandomTags(TagContainer, count);
	return RandomTags;
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
	APeCoGameMode* GameMode = CastChecked<APeCoGameMode>(GetOwner());
	UPeCoGameInstance* GameInstance = CastChecked<UPeCoGameInstance>(GameMode->GetGameInstance());

	UCurveTable* UpgradeCurveTable = GameInstance->StatUpgradeCurveTable;
	if (!UpgradeCurveTable)
	{
		UE_LOG(LogTemp, Error, TEXT("플레이어 스탯 데이터 테이블 없음."));
		return -1;
	}
	FPeCoStatData TargetStat = PlayerState->GetStatByTag(StatTag);
	FName RowName = StatTag.GetTagName();
	FRealCurve* StatCurve = UpgradeCurveTable->FindCurve(RowName, TEXT(""));
	if (!StatCurve)
	{
		UE_LOG(LogTemp, Error, TEXT("커브 테이블에서 해당 Stat Curve를 찾지 못함!"));
		return -1;
	}
	int32 TargetStatLevel = TargetStat.GetStatLevel() + 1;
	float RewardValue = StatCurve->Eval(TargetStatLevel);

	return  RewardValue;
}

bool UStoreComponent::BuyItemByTag(FGameplayTag ItemTag, FText& OutNote)
{


	return false;
}

bool UStoreComponent::SellItemByTag(FGameplayTag ItemTag, FText& OutNote)
{



	return false;
}
