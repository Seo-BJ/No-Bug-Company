// Fill out your copyright notice in the Description page of Project Settings.


#include "PeCoPlayerState.h"

#include "00_GameModes/PeCoGameMode.h"
#include "00_GameModes/Components/StoreComponent.h"
#include "01_Character/PeCoPlayerCharacter.h"

#include "02_Player/PeCoPlayerController.h"
#include "02_Player/PlayerStats.h"
#include "02_Player/Components/PlayerStatPresenterComponent.h"
#include "04_UI/PeCoHUD.h"
#include "07_Weapon/Weapon.h"
#include "20_System/PeCoGameInstance.h"
#include "20_System/PeCoFunctionLibrary.h"
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
	CharacterStats.Add( MaxHealth);
	CharacterStats.Add( MoveSpeed);
	CharacterStats.Add(DamageResistance);
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
		Damage = FMath::Clamp(Damage - Damage*(GetDamageResistance()/100), 0, Damage);
		AddHealth(Damage * -1, InstigatorController, DamageCauser);
	}	
}
void APeCoPlayerState::AddHealth(float Amount, AController* InstigatorController, AActor* DamageCauser)
{
	float OldHealth = GetHealth();
	float NewHealth = FMath::Clamp(OldHealth + Amount, 0.f, GetMaxHealth());
	SetBaseHealth(NewHealth);
	HandleHealthChagne(Amount * -1, InstigatorController, DamageCauser);
	Health.OnStatChanged.Broadcast(OldHealth, NewHealth);
}

void APeCoPlayerState::HandleHealthChagne(float Damage, AController* InstigatorController, AActor* DamageCauser)
{
	APeCoPlayerCharacter* PlayerCharacter = GetPawn<APeCoPlayerCharacter>();
	if (!IsValid(PlayerCharacter)) return;
	APeCoPlayerController* PeCoPlayerController = GetPawn()->GetController<APeCoPlayerController>();
	if (!IsValid(PeCoPlayerController)) return;

	// 데미지를 입는 경우
	if (Damage > 0)
	{
		PlayerCharacter->ShakeCameraWhenDamaged();
		PlayerCharacter->StartAlphaFade(PlayerCharacter->DamageMaterialInstance, PlayerCharacter->CrashInvincibleDuration);

		ShowFloatingText(PeCoPlayerController->GetPawn(), InstigatorController, Damage);
		PeCoPlayerController->ShowDamageScreenWidget();	
	}
	else
	{
		ShowFloatingText(PeCoPlayerController->GetPawn(), InstigatorController, Damage);
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
	UPeCoGameInstance* GameInstance = GetGameInstance<UPeCoGameInstance>();
	if (!IsValid(GameInstance))
	{
		return;
	}
	UDataTable* InitStatDataTable = GameInstance->PlayerInitStatDataTable;
	FPeCoStatData TargetStat;
	if (!InitStatDataTable)
	{
		UE_LOG(LogTemp, Error, TEXT("플레이어 스탯 데이터 테이블 없음."));
		return;
	}
	check(PlayerStatPresenterComponent);
	PlayerStatPresenterComponent->BindCallbacksToDependencies();
	// To Do:행 이름 변경
	FName RowName = FName("0");
	FPlayerStatData* RowData = InitStatDataTable->FindRow<FPlayerStatData>(RowName, TEXT("Read Player Init Stat"), true); // Warn if not found
	if (RowData)
	{
		InitMaxHealth(RowData->MaxHealth);
		InitHealth(RowData->MaxHealth);
		InitMoveSpeed(RowData->MoveSpeed);
		InitDamageResistance(RowData->DamageResistance);
		InitSkillCoolTime(RowData->SkillCoolTime);

		PlayerStatPresenterComponent->BroadcastInitialValues();
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Row '0' not found in PlayerInitStatDataTable"));
	}
}



void APeCoPlayerState::UpgradeStat(FGameplayTag StatTag)
{
	AGameModeBase* GameMode = UGameplayStatics::GetGameMode(this);
	if (!IsValid(GameMode))
	{
		return;
	}
	APeCoGameMode* PeCoGameMode = CastChecked<APeCoGameMode>(GameMode);
	UStoreComponent* StoreComponent = PeCoGameMode->FindComponentByClass<UStoreComponent>();
	if (!IsValid(StoreComponent))
	{
		return;
	}
	APeCoPlayerController* PeCoPlayerController = GetPawn()->GetController<APeCoPlayerController>();
	if (!IsValid(PeCoPlayerController))
	{
		return;
	}
	APeCoPlayerCharacter* PlayerCharacter = PeCoPlayerController->GetPawn<APeCoPlayerCharacter>();
	if (!IsValid(PlayerCharacter))
	{
		return;
	}
	
	float UpgradeAmount = StoreComponent->GetStatUpgradeData(StatTag, Cast<APeCoPlayerController>(GetOwningController()));

	if (StatTag.MatchesTagExact(PeCoGameplayTags::PlayerStat_Health)) return;
	if (StatTag.MatchesTagExact(PeCoGameplayTags::PlayerStat_MaxHealth))
	{
		MaxHealth.AddAdditiveBonus(UpgradeAmount);
		MaxHealth.IncreaseLevel(1);
	}
	else if (StatTag.MatchesTagExact(PeCoGameplayTags::PlayerStat_MoveSpeed))
	{
		MoveSpeed.AddMultiplierBonus(UpgradeAmount/100);	
		MoveSpeed.IncreaseLevel(1);
		PlayerCharacter->GetCharacterMovement()->MaxWalkSpeed = GetMoveSpeed();
		PlayerCharacter->GetCharacterMovement()->MaxWalkSpeedCrouched = GetMoveSpeed();
	}
	else if (StatTag.MatchesTagExact(PeCoGameplayTags::PlayerStat_DamageResistance))
	{
		DamageResistance.AddAdditiveBonus(UpgradeAmount);
		DamageResistance.IncreaseLevel(1);
	}
	else if (StatTag.MatchesTagExact(PeCoGameplayTags::PlayerStat_SkillCoolTime))
	{
		SkillCoolTime.AddAdditiveBonus(-UpgradeAmount);
		SkillCoolTime.IncreaseLevel(1);
	}
	else
	{
		PlayerCharacter->PlayerWeapon->UpgradeWeapon(StatTag, UpgradeAmount);
	}
}



void APeCoPlayerState::CheckLevelUp()
{
	UPeCoGameInstance* GameInstance = GetGameInstance<UPeCoGameInstance>();
	if (!IsValid(GameInstance))
	{
		return;
	}
	UCurveTable* LevelUpCurveTable = GameInstance->LevelUpCurveTable;
	FPeCoStatData TargetStat;
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
	UPeCoGameInstance* GameInstance = GetGameInstance<UPeCoGameInstance>();
	if (!IsValid(GameInstance))
	{
		return;
	}
	APeCoPlayerController* PeCoPlayerController = GetPawn()->GetController<APeCoPlayerController>();
	if (!IsValid(PeCoPlayerController))
	{
		return;
	}
	APeCoPlayerCharacter* PeCoPlayerChracter = PeCoPlayerController->GetPawn<APeCoPlayerCharacter>();
	if (!IsValid(PeCoPlayerChracter))
	{
		return;
	}
	UDataTable* InitStatDataTable = GameInstance->SupplyDataTable;
	FPeCoStatData TargetStat;
	if (!InitStatDataTable)
	{
		UE_LOG(LogTemp, Error, TEXT("보급품 데이터 테이블 없음."));
		return;
	}

	Level = NewLevel;
	TMap<FGameplayTag, int32> SupplyResult;
	TArray<FSupplyData*> AllRows;
	InitStatDataTable->GetAllRows<FSupplyData>(TEXT("Read Supply Data"), AllRows);
	FSupplyData* TargetRow = nullptr;
	for (auto Row : AllRows)
	{
		if (Row->LastLevel >= Level)
		{
			TargetRow = Row;
			break;
		}
	}
	if (TargetRow)
	{

		for (auto SupplyProbability : TargetRow->SupplyMap)
		{
			if (SupplyProbability.Probablities.Num() != SupplyProbability.Amounts.Num())
			{
				UE_LOG(LogTemp, Error, TEXT("보급품 데이터 테이블 입력 오류."));
				return;
			}
			int32 ItemAmount = 0;
			int32 TotalProbability = 0;
			for (auto Probability : SupplyProbability.Probablities)
			{
				TotalProbability += Probability;
			}
			for (int32 i = 0; i < SupplyProbability.Probablities.Num(); i++)
			{
				int32 RandomInt = FMath::RandRange(1, 100);
				int32 ProbabilitySum = 0;
				if (RandomInt > TotalProbability)
				{
					break;
				}

				if (RandomInt <= ProbabilitySum + SupplyProbability.Probablities[i])
				{
					ItemAmount = SupplyProbability.Amounts[i];
				}
				else
				{
					ProbabilitySum += SupplyProbability.Probablities[i];
					continue;
				}
			}
	
			UInventoryComponent* InventoryComponent = UPeCoFunctionLibrary::GetInventoryComponent(PeCoPlayerChracter);
			if (IsValid(InventoryComponent))
			{
				FText OutNote;
				InventoryComponent->AddItemsOfClass(SupplyProbability.SupplyActorClass, ItemAmount, OutNote);
				SupplyResult.Add(SupplyProbability.ItemTag, ItemAmount);
			}
		}	
	}
	PeCoPlayerController->ShowSupplyResultWidget(SupplyResult);
    PeCoPlayerController->SetPause(true);
	PeCoPlayerController = PeCoPlayerController == nullptr ? Cast<APeCoPlayerController>(GetPawn()->GetController()) : PeCoPlayerController;
	OnLevelChanged.Broadcast(Level);
    
}
uint32 APeCoPlayerState::GetCurrentLevelKillCount(uint32 CurrentLevel, uint32 CurrentKillCount)
{
	UPeCoGameInstance* GameInstance = GetGameInstance<UPeCoGameInstance>();
	if (!IsValid(GameInstance))
	{
		return -1;
	}
	UCurveTable* LevelUpCurveTable = GameInstance->LevelUpCurveTable;
	FPeCoStatData TargetStat;
	if (!LevelUpCurveTable)
	{
		UE_LOG(LogTemp, Warning, TEXT("레벨 업 커브 테이블 없음"));
		return -1;
	}
	float AccumulatedKillCount = 0.0f; 
	for (uint32 LevelIndex = 1; LevelIndex < CurrentLevel; ++LevelIndex)
	{
		FName RowName = FName("PlayerLevelUpCurve"); 
		FRealCurve* LevelCurve = LevelUpCurveTable->FindCurve(RowName, TEXT(""));
		if (!LevelCurve)
		{
			UE_LOG(LogTemp, Warning, TEXT("커브 테이블에서 PlayerLevelUpCurve를 찾지 못함!"));
			return -1;
		}
		float RequiredKillCountPerLevel = LevelCurve->Eval(LevelIndex);
		AccumulatedKillCount += RequiredKillCountPerLevel;
	}
	float CurrentLevelExperience = CurrentKillCount - AccumulatedKillCount;
	return FMath::CeilToInt(CurrentLevelExperience);
}
int32 APeCoPlayerState::GetCurrentLevelRequiredKillCount()
{
	UPeCoGameInstance* GameInstance = GetGameInstance<UPeCoGameInstance>();
	if (!IsValid(GameInstance))
	{
		return -1;
	}
	UCurveTable* LevelUpCurveTable = GameInstance->LevelUpCurveTable;
	FPeCoStatData TargetStat;
	if (!LevelUpCurveTable)
	{
		UE_LOG(LogTemp, Warning, TEXT("레벨 업 커브 테이블 없음"));
		return -1;
	}
	FName RowName = FName("PlayerLevelUpCurve");
	FRealCurve* LevelCurve = LevelUpCurveTable->FindCurve(RowName, TEXT(""));
	if (!LevelCurve)
	{
		UE_LOG(LogTemp, Warning, TEXT("커브 테이블에서 PlayerLevelUpCurve를 찾지 못함!"));
		return -1;
	}

	float CurrentLevelExperience = LevelCurve->Eval(Level);
	return FMath::CeilToInt(CurrentLevelExperience);
}

FPeCoStatData APeCoPlayerState::GetStatByTag(FGameplayTag Tag)
{
	if (Tag == PeCoGameplayTags::PlayerStat_MaxHealth)
	{
		return MaxHealth;
	}
	else if (Tag == PeCoGameplayTags::PlayerStat_MoveSpeed)
	{
		return MoveSpeed;
	}
	else if (Tag == PeCoGameplayTags::PlayerStat_DamageResistance)
	{
		return DamageResistance;
	}
	else if (Tag == PeCoGameplayTags::PlayerStat_SkillCoolTime)
	{
		return SkillCoolTime;
	}
	else if (Tag == PeCoGameplayTags::PlayerStat_Health)
	{
		return Health;
	}
	return FPeCoStatData();
}
