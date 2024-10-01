// Fill out your copyright notice in the Description page of Project Settings.


#include "PeCoPlayerState.h"
#include "00_GameModes/PeCoGameMode.h"

#include "02_Player/PeCoPlayerController.h"

#include "06_Data/PeCoDataRow.h"

#include "Engine/DataTable.h"


APeCoPlayerState::APeCoPlayerState()
{
}

void APeCoPlayerState::AddToKillCount(int32 KillCountAmount)
{
	// To do : KillCountAmount가 음수인 경우 0으로 설정 ?
	SetKillCount(GetKillCount() + KillCountAmount);
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

void APeCoPlayerState::CheckLevelUp()
{
    if (!LevelUpDataTable)
    {
        UE_LOG(LogTemp, Warning, TEXT("레벨 업 데이터 테이블 없음"));
        return;
    }

    static const FString ContextString(TEXT("Level Up Context"));

    // 레벨에 해당하는 데이터 테이블의 행을 찾기
    FLevelUpData* NextLevelData = LevelUpDataTable->FindRow<FLevelUpData>(FName(*FString::FromInt(CurrentLevel + 1)), ContextString, true);

    if (NextLevelData && KillCount >= NextLevelData->RequiredKillCount)
    {
        // 레벨업 처리
        HandleLevelUp(CurrentLevel + 1);
    }
}

void APeCoPlayerState::HandleLevelUp(int32 NewLevel)
{
    CurrentLevel = NewLevel;
    UE_LOG(LogTemp, Log, TEXT("Level Up! New Level: %d"), CurrentLevel);
   
    if (APlayerController* PC = GetWorld()->GetFirstPlayerController())
    {
        APeCoPlayerController* PeCoPlayerController = Cast<APeCoPlayerController>(PC);
        if (PeCoPlayerController)
        {
            PeCoPlayerController->SetPause(true);
            // PC->ShowLevelUpUI();
        }

    }

}

void APeCoPlayerState::ReceiveDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType, AController* InstigatorController, AActor* DamageCauser)
{
	PeCoGameMode = PeCoGameMode == nullptr ? GetWorld()->GetAuthGameMode<APeCoGameMode>() : PeCoGameMode;
	check(PeCoGameMode);
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

	Health = FMath::Clamp(Health - DamageToHealth, 0.f, MaxHealth);

	// UpdateHUDHealth();
	// UpdateHUDShield();
	// PlayHitReactMontage();

	if (Health <= 0.f)
	{
		PeCoGameMode = PeCoGameMode == nullptr ? GetWorld()->GetAuthGameMode<APeCoGameMode>() : PeCoGameMode;
		if (PeCoGameMode)
		{
			CharacterDie();
		}
	}

}

void APeCoPlayerState::CharacterDie()
{
	Destroy();
	// To Do : 그 외 플레이어 사망 이벤트 처리
}
