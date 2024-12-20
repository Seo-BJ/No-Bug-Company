// Fill out your copyright notice in the Description page of Project Settings.


#include "PeCoGameplayTags.h"


namespace PeCoGameplayTags
{
	UE_DEFINE_GAMEPLAY_TAG(Item, "Item");
	UE_DEFINE_GAMEPLAY_TAG(Item_Consumption, "Item.Consumption");
	UE_DEFINE_GAMEPLAY_TAG(Item_Combat, "Item.Combat");
	UE_DEFINE_GAMEPLAY_TAG(Item_Material, "Item.Material");

	UE_DEFINE_GAMEPLAY_TAG(Item_Consumption_EnergyDrink, "Item.Consumption.EnergyDrink");
	UE_DEFINE_GAMEPLAY_TAG(Item_Consumption_FirstAidKit, "Item.Consumption.FirstAidKit");
	UE_DEFINE_GAMEPLAY_TAG(Item_Consumption_Speed, "Item.Consumption.Speed");
	UE_DEFINE_GAMEPLAY_TAG(Item_Consumption_Power, "Item.Consumption.Power");
	UE_DEFINE_GAMEPLAY_TAG(Item_Consumption_Adrenaline, "Item.Consumption.Adrenaline");
	UE_DEFINE_GAMEPLAY_TAG(Item_Consumption_AntiSpray, "Item.Consumption.AntiSpray");
	
	UE_DEFINE_GAMEPLAY_TAG(Item_Combat_Mine, "Item.Combat.Mine");
	UE_DEFINE_GAMEPLAY_TAG(Item_Combat_Adhesive, "Item.Combat.Adhesive");
	UE_DEFINE_GAMEPLAY_TAG(Item_Combat_Grenade, "Item.Combat.Grenade");

	UE_DEFINE_GAMEPLAY_TAG(Item_Material_MechanicalComponent, "Item.Material.MechanicalComponent");
	UE_DEFINE_GAMEPLAY_TAG(Item_Material_BiologicalSample, "Item.Material.BiologicalSample");
	UE_DEFINE_GAMEPLAY_TAG(Item_Material_FlameBioSample, "Item.Material.FlameBioSample");
	UE_DEFINE_GAMEPLAY_TAG(Item_Material_VirusSample, "Item.Material.VirusSample");
	UE_DEFINE_GAMEPLAY_TAG(Item_Material_WebSample, "Item.Material.WebSample");

	UE_DEFINE_GAMEPLAY_TAG(Weapon_Conical_Pesticide, "Weapon.Conical.Pesticide");
	UE_DEFINE_GAMEPLAY_TAG(Weapon_Conical_Flamethrower, "Weapon.Conical.Flamethrower");
	UE_DEFINE_GAMEPLAY_TAG(Weapon_Projectile_AirGun, "Weapon.Projectile.AirGun");
	UE_DEFINE_GAMEPLAY_TAG(Weapon_Projectile_LarvaLauncher, "Weapon.Projectile.LarvaLauncher");
	UE_DEFINE_GAMEPLAY_TAG(Weapon_Projectile_PestShotgun, "Weapon.Projectile.PestShotgun");
	UE_DEFINE_GAMEPLAY_TAG(Weapon_Projectile_RoachShooter, "Weapon.Projectile.RoachShooter");
	UE_DEFINE_GAMEPLAY_TAG(Weapon_Projectile_WebRevolver, "Weapon.Projectile.WebRevolver");

	UE_DEFINE_GAMEPLAY_TAG(PlayerStat, "PlayerStat");
	UE_DEFINE_GAMEPLAY_TAG(PlayerStat_Health, "PlayerStat.Health");
	UE_DEFINE_GAMEPLAY_TAG(PlayerStat_MaxHealth, "PlayerStat.MaxHealth");
	UE_DEFINE_GAMEPLAY_TAG(PlayerStat_MoveSpeed, "PlayerStat.MoveSpeed");
	UE_DEFINE_GAMEPLAY_TAG(PlayerStat_DamageResistance, "PlayerStat.DamageResistance");
	UE_DEFINE_GAMEPLAY_TAG(PlayerStat_SkillCoolTime, "PlayerStat.SkillCoolTime");

	UE_DEFINE_GAMEPLAY_TAG(WeaponStat, "WeaponStat");
	UE_DEFINE_GAMEPLAY_TAG(WeaponStat_Damage, "WeaponStat.Damage");
	UE_DEFINE_GAMEPLAY_TAG(WeaponStat_AttackPower, "WeaponStat.AttackPower");
	UE_DEFINE_GAMEPLAY_TAG(WeaponStat_AttackSpeed, "WeaponStat.AttackSpeed");
	UE_DEFINE_GAMEPLAY_TAG(WeaponStat_CriticalChance, "WeaponStat.CriticalChance");
	UE_DEFINE_GAMEPLAY_TAG(WeaponStat_CriticalDamage, "WeaponStat.CriticalDamage");
	UE_DEFINE_GAMEPLAY_TAG(WeaponStat_Range, "WeaponStat.Range");
	UE_DEFINE_GAMEPLAY_TAG(WeaponStat_MaxAmmo, "WeaponStat.MaxAmmo");

	UE_DEFINE_GAMEPLAY_TAG(Enemy, "Enemy");
	UE_DEFINE_GAMEPLAY_TAG(Enemy_Spider, "Enemy.Spider.Normal");
	UE_DEFINE_GAMEPLAY_TAG(Enemy_Spider_Normal, "Enemy.Spider.Normal");
	UE_DEFINE_GAMEPLAY_TAG(Enemy_Spider_Boss, "Enemy.Spider.Boss");
	UE_DEFINE_GAMEPLAY_TAG(Enemy_Mosquito, "Enemy.Mosquito");
	UE_DEFINE_GAMEPLAY_TAG(Enemy_Mosquito_Normal, "Enemy.Mosquito.Normal");


	FGameplayTagContainer GetRandomTags(const FGameplayTagContainer& TargetTagContainer, int Count)
	{
		TArray<FGameplayTag> TagsArray;
		TargetTagContainer.GetGameplayTagArray(TagsArray);
		FGameplayTagContainer RandomTags;
	
		if (Count <= 0 || TagsArray.Num() == 0)
		{
			return RandomTags;
		}

		int32 MaxCount = FMath::Min(Count, TagsArray.Num());

		while (RandomTags.Num() < MaxCount)
		{
			int32 RandomIndex = FMath::RandRange(0, TagsArray.Num() - 1);
			RandomTags.AddTag(TagsArray[RandomIndex]);
			TagsArray.RemoveAt(RandomIndex); 
		}

		return RandomTags;
	}
	PESTCONTROL_API FGameplayTagContainer GetChildTags(const FGameplayTag& ParentTag)
	{
		// GameplayTagsManager�� ���� �±� �˻�
		const UGameplayTagsManager& TagManager = UGameplayTagsManager::Get();

		// �ڽ� �±� �˻�
		return TagManager.RequestGameplayTagChildren(ParentTag);
	}
}
