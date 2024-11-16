// Fill out your copyright notice in the Description page of Project Settings.


#include "PeCoPlayerState.h"

#include "00_GameModes/PeCoGameMode.h"
#include "01_Character/PeCoPlayerCharacter.h"

#include "02_Player/PeCoPlayerController.h"
#include "02_Player/PlayerStats.h"
#include "02_Player/Components/PlayerStatPresenterComponent.h"
#include "04_UI/PeCoHUD.h"
#include "21_Data/PeCoDataRow.h"

#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerState.h"
#include "Engine/CurveTable.h"
#include "Engine/DataTable.h"



APeCoPlayerState::APeCoPlayerState()
{
	SetTeam(ETeam::ET_Player);

	PlayerStatPresenterComponent = CreateDefaultSubobject<UPlayerStatPresenterComponent>(TEXT("StatPresenter"));
	CharacterStats.Add(Health);
	CharacterStats.Add(MaxHealth);
	CharacterStats.Add(MoveSpeed);
	CharacterStats.Add(AttackPower);
	CharacterStats.Add(AttackSpeed);
	CharacterStats.Add(DamageResistance);
	CharacterStats.Add(CriticalChance);
	CharacterStats.Add(CriticalDamage);
	CharacterStats.Add(Range);
	CharacterStats.Add(SkillCoolTime);
}
void APeCoPlayerState::BeginPlay()
{
	Super::BeginPlay();
}

void APeCoPlayerState::ReceiveDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType, AController* InstigatorController, AActor* DamageCauser)
{
	ICombatInterface::ReceiveDamage(DamagedActor, Damage, DamageType, InstigatorController, DamageCauser);
	APeCoGameMode* PeCoGameMode = GetWorld()->GetAuthGameMode<APeCoGameMode>();
	if (IsValid(PeCoGameMode))
	{
		Damage = PeCoGameMode->CalculateDamage(InstigatorController, GetPawn()->GetController(), Damage);
		float DamageToHealth = Damage;
		/*
		if (Shield > 0.f)
		{
			if (Shield >= Damage)
			{
				Shield = FMath::Clamp(Shield - Damage, 0.f, MaxShield);
				DamageToHealth = 0.f;
			}
			else
			{
				DamageToHealth = FMath::Clamp(DamageToHealth - Shield, 0.f, Damage);
				Shield = 0.f;
			}
		}
		*/
		AddHealth(Damage * -1, InstigatorController, DamageCauser);
	}	
}
void APeCoPlayerState::AddHealth(float Amount, AController* InstigatorController, AActor* DamageCauser)
{
	float OldHealth = GetHealth();
	float NewHealth = FMath::Clamp(OldHealth + Amount, 0.f, GetMaxHealth());
	SetBaseHealth(NewHealth);
	HandleHealthChagne(Amount * -1, InstigatorController, DamageCauser);
	OnHealthChagned.Broadcast(OldHealth, NewHealth);
}
void APeCoPlayerState::HandleHealthChagne(float Damage, AController* InstigatorController, AActor* DamageCauser)
{
	// 데미지를 입는 경우
	if (Damage > 0)
	{
		ShowFloatingText(GetPlayerController()->GetPawn(), InstigatorController, Damage);
	}

	// 죽음 처리
	if (GetHealth() <= 0.f)
	{
		APeCoGameMode* PeCoGameMode = GetWorld()->GetAuthGameMode<APeCoGameMode>();
		if (IsValid(PeCoGameMode))
		{
			CharacterDie();
		}
	}
}
void APeCoPlayerState::CharacterDie()
{
	APeCoPlayerController* PeCoPlayerController = GetPawn()->GetController<APeCoPlayerController>();
	if (IsValid(PeCoPlayerController))
	{
		APeCoPlayerCharacter* PlayerCharacter = PeCoPlayerController->GetPawn<APeCoPlayerCharacter>();
		if (IsValid(PlayerCharacter))
		{
			PlayerCharacter->GetMesh()->SetSimulatePhysics(true);
			PlayerCharacter->GetMesh()->SetEnableGravity(true);
			PlayerCharacter->GetMesh()->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
			PlayerCharacter->GetMesh()->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Block);

			PeCoPlayerController->DisableInput(PeCoPlayerController);

			// Option
			// PlayerCharacter->GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

			//Dissolve();
			//bDead = true;
		}

		APeCoHUD* PeCoHUD = Cast<APeCoHUD>(PeCoPlayerController->GetHUD());
		if (IsValid(PeCoHUD))
		{
			PeCoHUD->AddGameOverWidget();

			PeCoPlayerController->SetInputMode(FInputModeUIOnly());
			PeCoPlayerController->bShowMouseCursor = true;
		}
	}

	// To Do: 추가 Death Event 처리
}

void APeCoPlayerState::AddToKillCount(int32 KillCountAmount)
{
	// To do : KillCountAmount가 음수인 경우 0으로 설정 ?
	SetKillCount(GetKillCount() + KillCountAmount);
	OnExpChanged.Broadcast(GetCurrentLevelKillCount(Level, KillCount));
	CheckLevelUp();
}
int32 APeCoPlayerState::GetKillCount()
{
	return KillCount;
}
void APeCoPlayerState::SetKillCount(int32 KillCountAmount)
{
	KillCount = KillCountAmount;

}

void APeCoPlayerState::InitPlayerStat()
{
	if (!PlayerInitStatDataTable)
	{
		UE_LOG(LogTemp, Error, TEXT("플레이어 스탯 데이터 테이블 없음."));
		return;
	}

	PlayerStatPresenterComponent->BindCallbacksToDependencies();
	// To Do:행 이름 변경
	FName RowName = FName("0");
	FPlayerStatData* RowData = PlayerInitStatDataTable->FindRow<FPlayerStatData>(RowName, TEXT("Read Player Init Stat"), true); // Warn if not found
	if (RowData)
	{
		InitMaxHealth(RowData->MaxHealth);
		InitHealth(RowData->MaxHealth);
		InitMoveSpeed(RowData->MoveSpeed);
		InitAttackPower(RowData->AttackPower);
		InitAttackSpeed(RowData->AttackSpeed);
		InitDamageResistance(RowData->DamageResistance);
		InitCriticalChance(RowData->CriticalChance);
		InitCriticalDamage(RowData->CriticalDamage);
		InitRange(RowData->Range);
		InitSkillCoolTime(RowData->SkillCoolTime);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Row '0' not found in PlayerInitStatDataTable"));
	}
}

int32 APeCoPlayerState::GetStatUpgradeData(FGameplayTag StatTag)
{
	FPeCoStatData TargetStat;
	if (!StatUpgradeCurveTable)
	{
		UE_LOG(LogTemp, Error, TEXT("플레이어 스탯 데이터 테이블 없음."));
		return -1;
	}
	for (const auto Stat : CharacterStats)
	{
		if (Stat.StatTag.MatchesTagExact(StatTag))
		{
			TargetStat = Stat;
			break;
		}
	}
	FName RowName = StatTag.GetTagName(); 
	FRealCurve* StatCurve = StatUpgradeCurveTable->FindCurve(RowName, TEXT(""));
	if (!StatCurve)
	{
		UE_LOG(LogTemp, Error, TEXT("커브 테이블에서 해당 Stat Curve를 찾지 못함!"));
		return -1;
	}
	int32 TargetStatLevel = TargetStat.GetStatLevel() + 1;
	float RewardValue = StatCurve->Eval(TargetStatLevel);

	return  RewardValue;
}

void APeCoPlayerState::UpgradeStat(FGameplayTag StatTag)
{
	int Value = GetStatUpgradeData(StatTag);
	if (StatTag == PeCoGameplayTags::PlayerStat_MaxHealth)
	{
		MaxHealth.AddAdditiveBonus(Value);
	}
	else if (StatTag == PeCoGameplayTags::PlayerStat_MoveSpeed)
	{
		MoveSpeed.AddAdditiveBonus(Value);
	}
	else if (StatTag == PeCoGameplayTags::PlayerStat_AttackPower)
	{
		AttackPower.AddAdditiveBonus(Value);
	}
	else if (StatTag == PeCoGameplayTags::PlayerStat_AttackSpeed)
	{
		AttackSpeed.AddAdditiveBonus(Value);
	}
	else if (StatTag == PeCoGameplayTags::PlayerStat_DamageResistance)
	{
		DamageResistance.AddAdditiveBonus(Value);
	}
	else if (StatTag == PeCoGameplayTags::PlayerStat_CriticalChance)
	{
		CriticalChance.AddAdditiveBonus(Value);
	}
	else if (StatTag == PeCoGameplayTags::PlayerStat_CriticalDamage)
	{
		CriticalDamage.AddAdditiveBonus(Value);
	}
	else if (StatTag == PeCoGameplayTags::PlayerStat_Range)
	{
		Range.AddAdditiveBonus(Value);
	}
	else if (StatTag == PeCoGameplayTags::PlayerStat_SkillCoolTime)
	{
		SkillCoolTime.AddAdditiveBonus(Value);
	}
}

FGameplayTagContainer APeCoPlayerState::GetRandomStat(const int NumberOfRewards)
{
	FGameplayTagContainer TagContainer;
	for (const auto Stat : CharacterStats)
	{
		if (!Stat.StatTag.MatchesTagExact(PeCoGameplayTags::PlayerStat_Health))
		{
			TagContainer.AddTag(Stat.StatTag);
		}
	}
	return PeCoGameplayTags::GetRandomTags(TagContainer, NumberOfRewards);
}

void APeCoPlayerState::CheckLevelUp()
{
    if (!LevelUpCurveTable)
    {
        UE_LOG(LogTemp, Error, TEXT("레벨 업 커브 테이블 없음"));
        return;
    }
	FName RowName = FName("PlayerLevelUpCurve");
	FRealCurve* LevelCurve = LevelUpCurveTable->FindCurve(RowName, TEXT(""));
	if (!LevelCurve)
	{
		UE_LOG(LogTemp, Error, TEXT("커브 테이블에서 PlayerLevelUpCurve를 찾지 못함!"));
		return;
	}

	uint32 CurrentLevel = Level;
	uint32 CurrentKillCount = KillCount;

	float RequiredExperience = LevelCurve->Eval(Level);

	if (GetCurrentLevelKillCount(CurrentLevel, CurrentKillCount) >= RequiredExperience)
	{
		HandleLevelUp(CurrentLevel + 1);
	}
    
    
}
void APeCoPlayerState::HandleLevelUp(int32 NewLevel)
{
	if (NewLevel - Level <= 0)
	{
		UE_LOG(LogTemp, Error, TEXT("New Level은 기존 Level 보다 커야함!"));
	}
    Level = NewLevel;
	APeCoPlayerController* PeCoPlayerController = GetPawn()->GetController<APeCoPlayerController>();
	if (IsValid(PeCoPlayerController))
    {
        PeCoPlayerController->SetPause(true);
		PeCoPlayerController = PeCoPlayerController == nullptr ? Cast<APeCoPlayerController>(GetPawn()->GetController()) : PeCoPlayerController;
		OnLevelChanged.Broadcast(Level);
    }
}



uint32 APeCoPlayerState::GetCurrentLevelKillCount(uint32 CurrentLevel, uint32 CurrentKillCount)
{
	if (!LevelUpCurveTable)
	{
		UE_LOG(LogTemp, Warning, TEXT("레벨 업 커브 테이블 없음"));
		return 0;
	}
	float AccumulatedKillCount = 0.0f; 
	for (uint32 LevelIndex = 1; LevelIndex < CurrentLevel; ++LevelIndex)
	{
		FName RowName = FName("PlayerLevelUpCurve"); 
		FRealCurve* LevelCurve = LevelUpCurveTable->FindCurve(RowName, TEXT(""));
		if (!LevelCurve)
		{
			UE_LOG(LogTemp, Warning, TEXT("커브 테이블에서 PlayerLevelUpCurve를 찾지 못함!"));
			return 0.0f;
		}
		float RequiredKillCountPerLevel = LevelCurve->Eval(LevelIndex);
		AccumulatedKillCount += RequiredKillCountPerLevel;
	}
	float CurrentLevelExperience = CurrentKillCount - AccumulatedKillCount;
	return FMath::CeilToInt(CurrentLevelExperience);
}



int32 APeCoPlayerState::GetCurrentLevelRequiredKillCount()
{
	if (!LevelUpCurveTable)
	{
		UE_LOG(LogTemp, Warning, TEXT("레벨 업 커브 테이블 없음"));
		return 0;
	}
	FName RowName = FName("PlayerLevelUpCurve");
	FRealCurve* LevelCurve = LevelUpCurveTable->FindCurve(RowName, TEXT(""));
	if (!LevelCurve)
	{
		UE_LOG(LogTemp, Warning, TEXT("커브 테이블에서 PlayerLevelUpCurve를 찾지 못함!"));
		return 0.0f;
	}

	float CurrentLevelExperience = LevelCurve->Eval(Level);
	return FMath::CeilToInt(CurrentLevelExperience);
}

