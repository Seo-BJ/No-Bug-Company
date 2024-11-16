// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerStatPresenterComponent.h"

#include "01_Character/PeCoPlayerCharacter.h"
#include "02_Player/PeCoPlayerController.h"
#include "02_Player/PeCoPlayerState.h"


#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"



UPlayerStatPresenterComponent::UPlayerStatPresenterComponent()
{
	PrimaryComponentTick.bCanEverTick = true;


}


// Called when the game starts
void UPlayerStatPresenterComponent::BeginPlay()
{
	Super::BeginPlay();



	
}


// Called every frame
void UPlayerStatPresenterComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);


}

void UPlayerStatPresenterComponent::BroadcastInitialValues()
{



}

void UPlayerStatPresenterComponent::BindCallbacksToDependencies()
{
	APeCoPlayerState* PlayerState = GetOwner<APeCoPlayerState>();
	if (IsValid(PlayerState))
	{
		PlayerState->OnHealthChagned.AddLambda(
			[this](const float OldValue, const float NewValue)
			{
				OnHealthChanged.Broadcast(OldValue, NewValue);
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

void UPlayerStatPresenterComponent::BuffSpeed(float BuffBaseSpeed, float BuffCrouchSpeed, float BuffTime)
{
	APeCoPlayerState* PeCoPlayerState = Cast<APeCoPlayerState>(GetOwner());
	if (PeCoPlayerState)
	{
		if (APeCoPlayerCharacter* PlayerCharcater = Cast<APeCoPlayerCharacter>(PeCoPlayerState->GetPawn()))
		{
			PlayerCharcater->GetWorldTimerManager().SetTimer(SpeedBuffTimer, this, &UPlayerStatPresenterComponent::ResetSpeeds, BuffTime);
			if (PlayerCharcater->GetCharacterMovement())
			{
				SetInitialSpeeds(PlayerCharcater->GetCharacterMovement()->MaxWalkSpeed, PlayerCharcater->GetCharacterMovement()->MaxWalkSpeedCrouched);
				PlayerCharcater->GetCharacterMovement()->MaxWalkSpeed = BuffBaseSpeed;
				PlayerCharcater->GetCharacterMovement()->MaxWalkSpeedCrouched = BuffCrouchSpeed;
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
			PlayerCharcater->GetCharacterMovement()->MaxWalkSpeed = InitialBaseSpeed;
			PlayerCharcater->GetCharacterMovement()->MaxWalkSpeedCrouched = InitialCrouchSpeed;
		}
	}
}
void UPlayerStatPresenterComponent::SetInitialSpeeds(float BaseSpeed, float CrouchSpeed)
{
	InitialBaseSpeed = BaseSpeed;
	InitialCrouchSpeed = CrouchSpeed;
}
void UPlayerStatPresenterComponent::UpgradeStat(FGameplayTag StatTag)
{
	APeCoPlayerState* PeCoPlayerState = Cast<APeCoPlayerState>(GetOwner());
	if (PeCoPlayerState)
	{
		PeCoPlayerState->UpgradeStat(StatTag);
	}
}
#pragma endregion

