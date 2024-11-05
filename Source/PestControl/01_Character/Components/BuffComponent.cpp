// Fill out your copyright notice in the Description page of Project Settings.

#include "BuffComponent.h"

#include "01_Character/PeCoPlayerCharacter.h"
#include "02_Player/PeCoPlayerState.h"

#include "GameFramework/CharacterMovementComponent.h"

// Sets default values for this component's properties
UBuffComponent::UBuffComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UBuffComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UBuffComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

#pragma region Health

void UBuffComponent::Heal(float HealAmount)
{
	APeCoPlayerState* PlayerState = GetPlayerState();
	if (IsValid(PlayerState) && GetOwner())
	{
		float NewHealth = FMath::Clamp(PlayerState->GetHealth() + HealAmount, 0, PlayerState->GetMaxHealth());
		PlayerState->SetHealth(NewHealth, GetOwner(), nullptr);
	}
}
void UBuffComponent::HealToMaxHealth()
{
	APeCoPlayerState* PlayerState = GetPlayerState();
	if (IsValid(PlayerState) && GetOwner())
	{
		float NewHealth = FMath::Clamp(PlayerState->GetMaxHealth(), 0, PlayerState->GetMaxHealth());
		PlayerState->SetHealth(NewHealth, GetOwner(), nullptr);
	}
}
void UBuffComponent::HealByPercent(float Percent)
{
	APeCoPlayerState* PlayerState = GetPlayerState();
	if (IsValid(PlayerState) && GetOwner())
	{
		float AmountToHeal = PlayerState->GetMaxHealth() * (Percent / 100);
		Heal(AmountToHeal);
	}
}

#pragma endregion

#pragma region Speed Buff

void UBuffComponent::BuffSpeed(float BuffBaseSpeed, float BuffCrouchSpeed, float BuffTime)
{
	if (APeCoPlayerCharacter* PlayerCharcater = GetPlayerCharacter())
	{
		PlayerCharcater->GetWorldTimerManager().SetTimer(SpeedBuffTimer, this, &UBuffComponent::ResetSpeeds, BuffTime);
		if (PlayerCharcater->GetCharacterMovement())
		{
			PlayerCharcater->GetCharacterMovement()->MaxWalkSpeed = BuffBaseSpeed;
			PlayerCharcater->GetCharacterMovement()->MaxWalkSpeedCrouched = BuffCrouchSpeed;
		}
	}
}
void UBuffComponent::ResetSpeeds()
{
	if (APeCoPlayerCharacter* PlayerCharcater = GetPlayerCharacter())
	{
		if (PlayerCharcater->GetCharacterMovement())
		{
			PlayerCharcater->GetCharacterMovement()->MaxWalkSpeed = InitialBaseSpeed;
			PlayerCharcater->GetCharacterMovement()->MaxWalkSpeedCrouched = InitialCrouchSpeed;
		}
	}
}
void UBuffComponent::SetInitialSpeeds(float BaseSpeed, float CrouchSpeed)
{
	InitialBaseSpeed = BaseSpeed;
	InitialCrouchSpeed = CrouchSpeed;
}

#pragma endregion







APeCoPlayerState* UBuffComponent::GetPlayerState()
{
	AActor* ComponentOwner = GetOwner();
	if (!IsValid(ComponentOwner)) return nullptr;

	APeCoPlayerCharacter* PeCoPlayerCharcater = Cast<APeCoPlayerCharacter>(ComponentOwner);
	if (!IsValid(PeCoPlayerCharcater)) return nullptr;

	APeCoPlayerState* PeCoPlayerState = PeCoPlayerCharcater->GetPlayerState<APeCoPlayerState>();
	if (!IsValid(PeCoPlayerState)) return nullptr;

	return PeCoPlayerState;
}

APeCoPlayerCharacter* UBuffComponent::GetPlayerCharacter()
{
	AActor* ComponentOwner = GetOwner();
	if (!IsValid(ComponentOwner)) return nullptr;

	APeCoPlayerCharacter* PeCoPlayerCharcater = Cast<APeCoPlayerCharacter>(ComponentOwner);
	if (!IsValid(PeCoPlayerCharcater)) return nullptr;

	return PeCoPlayerCharcater;
}
