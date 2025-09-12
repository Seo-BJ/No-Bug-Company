// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerStatPresenterComponent.h"

#include "01_Character/PeCoPlayerCharacter.h"
#include "02_Player/PeCoPlayerController.h"
#include "02_Player/PeCoPlayerState.h"

#include "07_Weapon/Weapon.h"

#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"

UPlayerStatPresenterComponent::UPlayerStatPresenterComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}
void UPlayerStatPresenterComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UPlayerStatPresenterComponent::BroadcastInitialValues()
{
	APeCoPlayerState* PlayerState = GetOwner<APeCoPlayerState>();
	if (IsValid(PlayerState))
	{
		OnHealthChanged.Broadcast(0, PlayerState->GetHealth());
		OnMaxHealthChanged.Broadcast(0, PlayerState->GetMaxHealth());
		OnMoveSpeedChanged.Broadcast(0, PlayerState->GetMoveSpeed());
		OnDamageResistanceChanged.Broadcast(0, PlayerState->GetDamageResistance());
		OnSkillCoolTimeChanged.Broadcast(0, PlayerState->GetSkillCoolTime());
	}
}
void UPlayerStatPresenterComponent::BindCallbacksToDependencies() const
{
	APeCoPlayerState* PlayerState = GetOwner<APeCoPlayerState>();
	if (IsValid(PlayerState))
	{
		PlayerState->Health.OnStatChanged.AddLambda(
			[this](const float OldValue, const float NewValue)
			{
				OnHealthChanged.Broadcast(OldValue, NewValue);
			}
		);
		PlayerState->MaxHealth.OnStatChanged.AddLambda(
			[this](const float OldValue, const float NewValue)
			{
				OnMaxHealthChanged.Broadcast(OldValue, NewValue);
			}
		);
		PlayerState->MoveSpeed.OnStatChanged.AddLambda(
			[this](const float OldValue, const float NewValue)
			{
				OnMoveSpeedChanged.Broadcast(OldValue, NewValue);
			}
		);
		PlayerState->DamageResistance.OnStatChanged.AddLambda(
			[this](const float OldValue, const float NewValue)
			{
				OnDamageResistanceChanged.Broadcast(OldValue, NewValue);
			}
		);
		PlayerState->SkillCoolTime.OnStatChanged.AddLambda(
			[this](const float OldValue, const float NewValue)
			{
				OnSkillCoolTimeChanged.Broadcast(OldValue, NewValue);
			}
		);
	}
}

void UPlayerStatPresenterComponent::AddHealth(float Amount, AActor* CauserActor)
{
	APeCoPlayerState* PlayerState = GetOwner<APeCoPlayerState>();
	if (IsValid(PlayerState))
	{
		PlayerState->AddHealth(Amount, PlayerState->GetOwningController(), CauserActor);
	}
}
void UPlayerStatPresenterComponent::MaximizeHealth(AActor* CauserActor)
{
	APeCoPlayerState* PlayerState = GetOwner<APeCoPlayerState>();
	if (IsValid(PlayerState))
	{
		PlayerState->AddHealth(PlayerState->GetMaxHealth(), PlayerState->GetOwningController(), CauserActor);
	}
}
void UPlayerStatPresenterComponent::MultiplyHealth(float Percent, AActor* CauserActor)
{
	APeCoPlayerState* PlayerState = GetOwner<APeCoPlayerState>();
	if (IsValid(PlayerState))
	{
		float Amount = PlayerState->GetMaxHealth() * (Percent / 100);
		PlayerState->AddHealth(Amount, PlayerState->GetOwningController(), CauserActor);
	}
}

#pragma region Speed Buff
void UPlayerStatPresenterComponent::BuffSpeed(float Percent, float BuffTime)
{
	APeCoPlayerState* PeCoPlayerState = Cast<APeCoPlayerState>(GetOwner());
	if (PeCoPlayerState)
	{
		if (APeCoPlayerCharacter* PlayerCharcater = Cast<APeCoPlayerCharacter>(PeCoPlayerState->GetPawn()))
		{
			PlayerCharcater->GetWorldTimerManager().SetTimer(SpeedBuffTimer, this, &UPlayerStatPresenterComponent::ResetSpeeds, BuffTime);
			if (PlayerCharcater->GetCharacterMovement())
			{
				PlayerCharcater->GetCharacterMovement()->MaxWalkSpeed = (1 + (Percent/100))* PeCoPlayerState->GetMoveSpeed();
				PlayerCharcater->GetCharacterMovement()->MaxWalkSpeedCrouched = (1 + (Percent / 100)) * PeCoPlayerState->GetMoveSpeed();
			}
		}
	}
}
void UPlayerStatPresenterComponent::ResetSpeeds()
{
	APeCoPlayerState* PeCoPlayerState = Cast<APeCoPlayerState>(GetOwner());
	if (PeCoPlayerState)
	{
		if (APeCoPlayerCharacter* PlayerCharcater = Cast<APeCoPlayerCharacter>(PeCoPlayerState->GetPawn()))
		{
			PlayerCharcater->GetCharacterMovement()->MaxWalkSpeed = PeCoPlayerState->GetMoveSpeed();
			PlayerCharcater->GetCharacterMovement()->MaxWalkSpeedCrouched = PeCoPlayerState->GetMoveSpeed();
		}
	}
}
#pragma endregion

void UPlayerStatPresenterComponent::UpgradeStat(FGameplayTag StatTag)
{
	APeCoPlayerState* PeCoPlayerState = Cast<APeCoPlayerState>(GetOwner());
	if (PeCoPlayerState)
	{
		PeCoPlayerState->UpgradeStat(StatTag);
	}
}

void UPlayerStatPresenterComponent::BuffWeaponStat(FGameplayTag StatTag, float Amount, float BuffTime, float Percent)
{
	FTimerHandle WeaponStatBuffTimer;

	APeCoPlayerState* PeCoPlayerState = Cast<APeCoPlayerState>(GetOwner());
	if (PeCoPlayerState)
	{
		if (APeCoPlayerCharacter* PlayerCharacter = Cast<APeCoPlayerCharacter>(PeCoPlayerState->GetPawn()))
		{
			if (PlayerCharacter->PlayerWeapon)
			{
				float CurrentValue = PlayerCharacter->PlayerWeapon->GetStatValueByTag(StatTag);
				PreviousWeaponStats.Add(StatTag, CurrentValue);

				if(Amount == 0)
				{ 
					float PercentMultiplier = Percent / 100;
					PlayerCharacter->PlayerWeapon->UpgradeWeapon(StatTag, CurrentValue*PercentMultiplier);
				}
				else
				{
					PlayerCharacter->PlayerWeapon->UpgradeWeapon(StatTag, Amount);
				}
				PlayerCharacter->GetWorldTimerManager().SetTimer(WeaponStatBuffTimer, [this, StatTag]()
					{
						ResetWeaponStat(StatTag);
					}, BuffTime, false);
			}
		}
	}
}

void UPlayerStatPresenterComponent::ResetWeaponStat(FGameplayTag StatTag)
{
	APeCoPlayerState* PeCoPlayerState = Cast<APeCoPlayerState>(GetOwner());
	if (PeCoPlayerState)
	{
		if (APeCoPlayerCharacter* PlayerCharacter = Cast<APeCoPlayerCharacter>(PeCoPlayerState->GetPawn()))
		{
			if (PlayerCharacter->PlayerWeapon && PreviousWeaponStats.Contains(StatTag))
			{
				float PreviousValue = PreviousWeaponStats[StatTag];
				float CurrentValue = PlayerCharacter->PlayerWeapon->GetStatValueByTag(StatTag);
				PlayerCharacter->PlayerWeapon->UpgradeWeapon(StatTag, PreviousValue - CurrentValue);

				PreviousWeaponStats.Remove(StatTag);
			}
		}
	}
}