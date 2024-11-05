// Fill out your copyright notice in the Description page of Project Settings.


#include "PeCoPlayerState.h"
#include "00_GameModes/PeCoGameMode.h"

#include "01_Character/PeCoPlayerCharacter.h"
#include "02_Player/PeCoPlayerController.h"

#include "04_UI/PeCoHUD.h"

#include "21_Data/PeCoDataRow.h"

#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerState.h"
#include "Engine/CurveTable.h"


APeCoPlayerState::APeCoPlayerState()
{
	Health = MaxHealth;
	SetTeam(ETeam::ET_Player);
}

void APeCoPlayerState::ReceiveDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType, AController* InstigatorController, AActor* DamageCauser)
{
	ICombatInterface::ReceiveDamage(DamagedActor, Damage, DamageType, InstigatorController, DamageCauser);
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
	float NewHealth = FMath::Clamp(Health - DamageToHealth, 0.f, MaxHealth);
	SetHealth(NewHealth, DamagedActor, InstigatorController);
}

void APeCoPlayerState::SetHealth(float NewHealth, AActor* DamagedActor, AController* InstigatorController)
{
	OnHealthChagned.Broadcast(Health, NewHealth, nullptr);
	float Damage = Health - NewHealth;
	Health = NewHealth;

	// 데미지를 입는 경우
	if (Damage > 0)
	{
		ShowFloatingText(DamagedActor, InstigatorController, Damage);
	}

	// 죽음 처리
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
	PeCoPlayerController = PeCoPlayerController == nullptr ? Cast<APeCoPlayerController>(GetPawn()->GetController()) : PeCoPlayerController;
	if (PeCoPlayerController)
	{
		APeCoPlayerCharacter* PlayerCharacter = PeCoPlayerController->GetPawn<APeCoPlayerCharacter>();
		if (PlayerCharacter)
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
		if (PeCoHUD)
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

void APeCoPlayerState::CheckLevelUp()
{
    if (!LevelUpCurveTable)
    {
        UE_LOG(LogTemp, Warning, TEXT("레벨 업 커브 테이블 없음"));
        return;
    }
	FName RowName = FName("PlayerLevelUpCurve");
	FRealCurve* LevelCurve = LevelUpCurveTable->FindCurve(RowName, TEXT(""));
	if (!LevelCurve)
	{
		UE_LOG(LogTemp, Warning, TEXT("커브 테이블에서 PlayerLevelUpCurve를 찾지 못함!"));
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
    Level = NewLevel;
    if (APlayerController* PC = GetWorld()->GetFirstPlayerController())
    {
		PeCoPlayerController = PeCoPlayerController == nullptr ? Cast<APeCoPlayerController>(GetPawn()->GetController()) : PeCoPlayerController; //Allow us to not cast multiple time
        if (PeCoPlayerController)
        {
            PeCoPlayerController->SetPause(true);
			PeCoPlayerController = PeCoPlayerController == nullptr ? Cast<APeCoPlayerController>(GetPawn()->GetController()) : PeCoPlayerController;
		
			APeCoHUD* PeCoHUD = Cast<APeCoHUD>(PeCoPlayerController->GetHUD());
			if (PeCoHUD)
			{
				PeCoHUD->AddLevelUpWidget();
				OnLevelChanged.Broadcast(Level);
			}
        }
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

