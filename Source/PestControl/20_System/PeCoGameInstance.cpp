// Fill out your copyright notice in the Description page of Project Settings.


#include "PeCoGameInstance.h"

#include "01_Character/Components/InventoryComponent.h"
#include "01_Character/PeCoPlayerCharacter.h"
#include "02_Player/PeCoPlayerState.h"

#include "07_Weapon/Weapon.h"
#include "09_Items/Components/PeCoItemComponent.h"
#include "20_System/PeCoFunctionLibrary.h"

UPeCoGameInstance::UPeCoGameInstance()
{

	WeaponStatMap.Add(PeCoGameplayTags::WeaponStat_AttackPower, 0);
	WeaponStatMap.Add(PeCoGameplayTags::WeaponStat_Damage, 0);
	WeaponStatMap.Add(PeCoGameplayTags::WeaponStat_AttackSpeed, 0);
	WeaponStatMap.Add(PeCoGameplayTags::WeaponStat_CriticalChance, 0);
	WeaponStatMap.Add(PeCoGameplayTags::WeaponStat_CriticalDamage, 0);
	WeaponStatMap.Add(PeCoGameplayTags::WeaponStat_Range, 0);
}

void UPeCoGameInstance::SavePlayerInfo(AActor* Player)
{
	UInventoryComponent* Inventory = UPeCoFunctionLibrary::GetInventoryComponent(Player);
	if (IsValid(Inventory))
	{
		AllItems.Empty();
		TArray<AActor*> InventoryItems = Inventory->GetAllItems();
		for (AActor* Item : InventoryItems)
		{
			UPeCoItemComponent* ItemComponent = UPeCoFunctionLibrary::GetItemComponent(Item);
			if (ItemComponent)
			{
				FItemSaveData SaveData;
				SaveData.ItemClass = Item->GetClass();
				SaveData.Quantity = ItemComponent->ItemInfo.CurrentQuantity;

				AllItems.Add(SaveData); // 데이터 저장
			}
		}


		Money = Inventory->GetPlayerMoney();
		UE_LOG(LogTemp, Warning, TEXT("Save All Item And Moneny"));
	}

	APeCoPlayerCharacter* Character = Cast<APeCoPlayerCharacter>(Player);
	if (!IsValid(Character)) return;
	APeCoPlayerState* PlayerState = Character->GetPlayerState<APeCoPlayerState>();
	if (!IsValid(PlayerState)) return;


	MaxHealth = PlayerState->MaxHealth;
	MoveSpeed = PlayerState->MoveSpeed;
	DamageResistance = PlayerState->DamageResistance;
	SkillCoolTime = PlayerState->SkillCoolTime;


	AWeapon* Weapon = Character->PlayerWeapon;
	if (!IsValid(Weapon)) return;

	WeaponStatMap.Add(PeCoGameplayTags::WeaponStat_AttackPower, *Weapon->WeaponStatLevelMap.Find((PeCoGameplayTags::WeaponStat_AttackPower)));
	WeaponStatMap.Add(PeCoGameplayTags::WeaponStat_AttackSpeed, *Weapon->WeaponStatLevelMap.Find((PeCoGameplayTags::WeaponStat_AttackSpeed)));
	WeaponStatMap.Add(PeCoGameplayTags::WeaponStat_CriticalChance, *Weapon->WeaponStatLevelMap.Find((PeCoGameplayTags::WeaponStat_CriticalChance)));
	WeaponStatMap.Add(PeCoGameplayTags::WeaponStat_CriticalDamage, *Weapon->WeaponStatLevelMap.Find((PeCoGameplayTags::WeaponStat_CriticalDamage)));
	WeaponStatMap.Add(PeCoGameplayTags::WeaponStat_Damage, *Weapon->WeaponStatLevelMap.Find((PeCoGameplayTags::WeaponStat_Damage)));
	WeaponStatMap.Add(PeCoGameplayTags::WeaponStat_Range, *Weapon->WeaponStatLevelMap.Find((PeCoGameplayTags::WeaponStat_Range)));

	DamageMultiplier = Weapon->DamageMultiplier;
	BaseDamage = Weapon->BaseDamage;
	CooldownMultiplier = Weapon->CooldownMultiplier;
	CriticalChance = Weapon->CriticalChance;
	CriticalDamageMultiplier = Weapon->CriticalDamageMultiplier;
	Range = Weapon->Range;




}

void UPeCoGameInstance::LoadPlayerInfo(AActor* Player)
{
	UInventoryComponent* Inventory = UPeCoFunctionLibrary::GetInventoryComponent(Player);
	if (IsValid(Inventory))
	{
		FText OutNote;
		Inventory->AddPlayerMoney(Money, OutNote);

		for (const FItemSaveData& ItemData : AllItems)
		{
			if (*ItemData.ItemClass)
			{
				Inventory->AddItemsOfClass(ItemData.ItemClass, ItemData.Quantity, OutNote);
				UE_LOG(LogTemp, Warning, TEXT("Loaded Item: %s, Quantity: %d"),
					*ItemData.ItemClass->GetName(), ItemData.Quantity);
			}
		}

		// 데이터 초기화
		AllItems.Empty();
		Money = 0;
	}


	APeCoPlayerCharacter* Character = Cast<APeCoPlayerCharacter>(Player);
	if (!IsValid(Character)) return;
	APeCoPlayerState* PlayerState = Character->GetPlayerState<APeCoPlayerState>();
	if (!IsValid(PlayerState)) return;

	PlayerState->MaxHealth = MaxHealth;
	PlayerState->MoveSpeed = MoveSpeed;
	PlayerState->DamageResistance = DamageResistance;
	PlayerState->SkillCoolTime = SkillCoolTime;

	PlayerState->InitHealth(MaxHealth.GetCurrentValue());

	PlayerState->PlayerStatPresenterComponent->BroadcastInitialValues();


	AWeapon* Weapon = Character->PlayerWeapon;
	if (!IsValid(Weapon)) return;

	Weapon->WeaponStatLevelMap.Reset();

	Weapon->WeaponStatLevelMap.Add(PeCoGameplayTags::WeaponStat_AttackPower, *WeaponStatMap.Find((PeCoGameplayTags::WeaponStat_AttackPower)));
	Weapon->WeaponStatLevelMap.Add(PeCoGameplayTags::WeaponStat_AttackSpeed, *WeaponStatMap.Find((PeCoGameplayTags::WeaponStat_AttackSpeed)));
	Weapon->WeaponStatLevelMap.Add(PeCoGameplayTags::WeaponStat_CriticalChance, *WeaponStatMap.Find((PeCoGameplayTags::WeaponStat_CriticalChance)));
	Weapon->WeaponStatLevelMap.Add(PeCoGameplayTags::WeaponStat_CriticalDamage, *WeaponStatMap.Find((PeCoGameplayTags::WeaponStat_CriticalDamage)));
	Weapon->WeaponStatLevelMap.Add(PeCoGameplayTags::WeaponStat_Damage, *WeaponStatMap.Find((PeCoGameplayTags::WeaponStat_Damage)));
	Weapon->WeaponStatLevelMap.Add(PeCoGameplayTags::WeaponStat_Range, *WeaponStatMap.Find((PeCoGameplayTags::WeaponStat_Range)));


	Weapon->DamageMultiplier = DamageMultiplier;
	Weapon->BaseDamage = BaseDamage;
	Weapon->CooldownMultiplier = CooldownMultiplier;
	Weapon->CriticalChance = CriticalChance;
	Weapon->CriticalDamageMultiplier = CriticalDamageMultiplier;
	Weapon->Range = Range;
}

