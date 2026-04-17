// Fill out your copyright notice in the Description page of Project Settings.


#include "PeCoPlayerState.h"

#include "00_GameModes/PeCoGameMode.h"
#include "00_GameModes/StageOneGameMode.h"
#include "00_GameModes/StageTwoGameMode.h"
#include "00_GameModes/StageThreeGameMode.h"

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
	APeCoGameMode* PeCoGameMode = Cast<APeCoGameMode>(UGameplayStatics::GetGameMode(this));
	if (IsValid(PeCoGameMode))
	{
		if (PeCoGameMode->OnStageTimeEnd.IsBound())
		{
			PeCoGameMode->OnStageTimeEnd.Clear();
		}
		PeCoGameMode->OnStageTimeEnd.AddUObject(this, &APeCoPlayerState::HandleStageEnd);
	}
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

void APeCoPlayerState::BuffMoveSpeed(float Percent, float BuffTime)
{
	APeCoPlayerCharacter* PlayerCharacter = GetPawn<APeCoPlayerCharacter>();
	if (!IsValid(PlayerCharacter)) return;

	if (ActiveMoveSpeedBuffMultiplier != 0.f)
	{
		GetWorldTimerManager().ClearTimer(MoveSpeedBuffTimer);
		MoveSpeed.AddMultiplierBonus(-ActiveMoveSpeedBuffMultiplier);
		ActiveMoveSpeedBuffMultiplier = 0.f;
	}

	const float Multiplier = Percent / 100.f;
	MoveSpeed.AddMultiplierBonus(Multiplier);
	ActiveMoveSpeedBuffMultiplier = Multiplier;

	PlayerCharacter->GetCharacterMovement()->MaxWalkSpeed = GetMoveSpeed();
	PlayerCharacter->GetCharacterMovement()->MaxWalkSpeedCrouched = GetMoveSpeed();

	GetWorldTimerManager().SetTimer(MoveSpeedBuffTimer, this, &APeCoPlayerState::ResetMoveSpeedBuff, BuffTime, false);
}

void APeCoPlayerState::ResetMoveSpeedBuff()
{
	if (ActiveMoveSpeedBuffMultiplier == 0.f) return;

	MoveSpeed.AddMultiplierBonus(-ActiveMoveSpeedBuffMultiplier);
	ActiveMoveSpeedBuffMultiplier = 0.f;

	APeCoPlayerCharacter* PlayerCharacter = GetPawn<APeCoPlayerCharacter>();
	if (!IsValid(PlayerCharacter)) return;

	PlayerCharacter->GetCharacterMovement()->MaxWalkSpeed = GetMoveSpeed();
	PlayerCharacter->GetCharacterMovement()->MaxWalkSpeedCrouched = GetMoveSpeed();
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
			GameOver();
		}
	}
}
void APeCoPlayerState::HandleStageEnd(int32 StageNumber)
{
	if (StageNumber == 1)
	{
		GameOver();
	}
	else if (StageNumber == 2)
	{
		APeCoGameMode* PeCoGameMode = GetWorld()->GetAuthGameMode<APeCoGameMode>();
		if (IsValid(PeCoGameMode))
		{
			PeCoGameMode->StageFinishAndStartNextStage();
		}
	}
	else if (StageNumber == 3)
	{

	}


}
void APeCoPlayerState::GameOver()
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
		}

		APeCoHUD* PeCoHUD = Cast<APeCoHUD>(PeCoPlayerController->GetHUD());
		if (IsValid(PeCoHUD))
		{
			PeCoHUD->AddGameOverWidget();

			PeCoPlayerController->SetInputMode(FInputModeUIOnly());
			PeCoPlayerController->bShowMouseCursor = true;
		}
	}
	
}

void APeCoPlayerState::AddToKillCount(int32 KillCountAmount)
{
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
    // 레벨업 유효성 검사: 새로운 레벨은 기존 레벨보다 높아야 합니다.
    if (NewLevel - Level <= 0)
    {
       UE_LOG(LogTemp, Error, TEXT("New Level은 기존 Level 보다 커야함!"));
       return; // 유효하지 않은 경우 함수 종료
    }

    // 필수 컴포넌트 및 데이터 유효성 검사
    UPeCoGameInstance* GameInstance = GetGameInstance<UPeCoGameInstance>();
    if (!IsValid(GameInstance)) return;

    APeCoPlayerController* PeCoPlayerController = GetPawn()->GetController<APeCoPlayerController>();
    if (!IsValid(PeCoPlayerController)) return;

    APeCoPlayerCharacter* PeCoPlayerChracter = PeCoPlayerController->GetPawn<APeCoPlayerCharacter>();
    if (!IsValid(PeCoPlayerChracter)) return;

    // GameInstance에서 보상(Supply) 데이터 테이블을 가져옵니다.
    UDataTable* SupplyDataTable = GameInstance->SupplyDataTable;
    if (!SupplyDataTable)
    {
       UE_LOG(LogTemp, Error, TEXT("보급품 데이터 테이블 없음."));
       return;
    }

    // 플레이어의 레벨을 새로운 레벨로 갱신합니다.
    Level = NewLevel;

    // UI에 표시할 보상 결과 TMap
    TMap<FGameplayTag, int32> SupplyResult;

    // 데이터 테이블의 모든 행(Row)을 가져옵니다.
    TArray<FSupplyData*> AllRows;
    SupplyDataTable->GetAllRows<FSupplyData>(TEXT("Read Supply Data"), AllRows);
	
    FSupplyData* TargetRow = nullptr;
    for (auto Row : AllRows)
    {
       // 데이터 테이블의 LastLevel 값이 현재 레벨보다 크거나 같으면, 해당 보상 규칙을 사용
       if (Row->LastLevel >= Level)
       {
          TargetRow = Row;
          break;
       }
    }

    // 현재 레벨에 맞는 보상 데이터를 찾은 경우
    if (TargetRow)
    {
       // 해당 레벨 구간의 보상 아이템 목록을 순회
       for (const FSupplyProbablity SupplyProbability : TargetRow->SupplyMap)
       {
          // 데이터 테이블 유효성 검사 (확률 배열과 수량 배열의 크기가 동일해야 함)
          if (SupplyProbability.Probabilities.Num() != SupplyProbability.Amounts.Num())
          {
              UE_LOG(LogTemp, Error, TEXT("보급품 데이터 테이블의 확률/수량 배열 크기가 일치하지 않습니다."));
              return;
          }

          int32 ItemAmount = 0; // 최종적으로 지급될 아이템 수량
          int32 TotalProbability = 0; // 모든 가중치의 합

          // 데이터에 정의된 모든 가중치 값을 더하여 총합을 계산
          for (auto Probability : SupplyProbability.Probabilities)
          {
             TotalProbability += Probability;
          }

          if (TotalProbability <= 0)
          {
             continue; // 가중치가 없으면 이 엔트리 스킵
          }

          // 가중치 총합 범위에서 랜덤 값을 뽑고, 누적 가중치 구간에 매핑 (Weighted Random Selection)
          int32 RandomInt = FMath::RandRange(1, TotalProbability);
          int32 ProbabilitySum = 0;
          for (int32 i = 0; i < SupplyProbability.Probabilities.Num(); i++)
          {
             // 현재 누적 가중치에 현재 인덱스의 가중치를 더함
             ProbabilitySum += SupplyProbability.Probabilities[i];
             // 랜덤 값이 누적 가중치보다 작거나 같으면 해당 아이템 수량을 선택하고 반복을 종료
             if (RandomInt <= ProbabilitySum)
             {
                ItemAmount = SupplyProbability.Amounts[i];
                break;
             }
          }
          
          // 결정된 아이템을 인벤토리에 추가합니다.
          if (ItemAmount > 0)
          {
              UInventoryComponent* InventoryComponent = UPeCoFunctionLibrary::GetInventoryComponent(PeCoPlayerChracter);
              if (IsValid(InventoryComponent))
              {
                 FText OutNote; 
                 InventoryComponent->AddItemsOfClass(SupplyProbability.SupplyActorClass, ItemAmount, OutNote);
                 // UI에 표시하기 위해 결과 맵에 추가합니다.
                 SupplyResult.Add(SupplyProbability.ItemTag, ItemAmount);
              }
          }
       }  
    }

    // 컨트롤러를 통해 UI에 보상 결과 위젯을 표시하도록 요청합니다.
    PeCoPlayerController->ShowSupplyResultWidget(SupplyResult);
    
    // 레벨 변경 이벤트를 다른 시스템에 알립니다 (UI 업데이트 등).
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
