// Fill out your copyright notice in the Description page of Project Settings.


#include "PeCoPlayerController.h"


#include "00_GameModes/PeCoGameMode.h"

#include "03_Input/InPutActionDataAsset.h"

#include "04_UI/PeCoHUD.h"
#include "04_UI/PlayerOverlay.h"
#include "04_UI/SubWidget/GameTimerWidget.h"
#include "04_UI/SubWidget/PeCoProgressBar.h"

#include "Components/ProgressBar.h"

#include "Components/InputComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputMappingContext.h"


#include "GameFramework/Pawn.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"

#include "Components/TextBlock.h"

#include "Blueprint/UserWidget.h"



APeCoPlayerController::APeCoPlayerController()
{
}

void APeCoPlayerController::BeginPlay()
{
	Super::BeginPlay();

	UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer());
	if (Subsystem != nullptr)
	{
		Subsystem->AddMappingContext(InputMappingContextMove, 0);
	}

	GetGameTimeData();
	PeCoHUD = Cast<APeCoHUD>(GetHUD());

	
}

void APeCoPlayerController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	SetHUDTime(DeltaTime);
}

void APeCoPlayerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

}

void APeCoPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer());
	Subsystem->ClearAllMappings();
	Subsystem->AddMappingContext(InputMappingContextMove, 0);
	UEnhancedInputComponent* PEI = Cast<UEnhancedInputComponent>(InputComponent);
	PEI->BindAction(InputActions->InputActionMove, ETriggerEvent::Triggered, this, &APeCoPlayerController::Move);
	PEI->BindAction(InputActions->InputActionDash, ETriggerEvent::Triggered, this, &APeCoPlayerController::Dash);
}


void APeCoPlayerController::Move(const FInputActionValue& Value)
{
	FVector2D MovementVector = Value.Get<FVector2D>();

	CurrentMoveDirection = MovementVector;

	const FRotator	Rotation = GetControlRotation();
	const FRotator	YawRotation(0, Rotation.Yaw, 0);

	const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);

	const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

	if (GetPawn())
	{
		GetPawn()->AddMovementInput(ForwardDirection, MovementVector.X);
		GetPawn()->AddMovementInput(RightDirection, MovementVector.Y);
	}

}

void APeCoPlayerController::Dash(const FInputActionValue& Value)
{

	if (bCanDash && !CurrentMoveDirection.IsNearlyZero())
	{
		FVector DashDirection = FVector(CurrentMoveDirection.X, CurrentMoveDirection.Y, 0.0f).GetSafeNormal();
		ACharacter* ControlledCharacter = Cast<ACharacter>(GetPawn());
		if (ControlledCharacter)
		{
			DashDistance = DashDirection * (DashVelocity * DashDuration);
			ControlledCharacter->GetCharacterMovement()->MaxWalkSpeed = DashDistance.Size();
			bCanDash = false;
			GetWorldTimerManager().SetTimer(DashTimer, this, &APeCoPlayerController::ResetDash, DashDuration, false);
		}	
	}
}

void APeCoPlayerController::ResetDash()
{
	ACharacter* ControlledCharacter = Cast<ACharacter>(GetPawn());

	if (ControlledCharacter)
	{
		ControlledCharacter->GetCharacterMovement()->MaxWalkSpeed = 600.f; 
	}
	GetWorldTimerManager().SetTimer(DashTimer, this, &APeCoPlayerController::CoolDownDash, DashCooldown, false);
}

void APeCoPlayerController::CoolDownDash()
{
	bCanDash = true;
}

void APeCoPlayerController::SetHUDHealthBar(float Health, float MaxHealth)
{
	PeCoHUD = PeCoHUD == nullptr ? Cast<APeCoHUD>(GetHUD()) : PeCoHUD;
	bool bHUDValid = PeCoHUD
		&& PeCoHUD->GetPlayerOverlayWidget()
		&& PeCoHUD->GetPlayerOverlayWidget()->HealthBar
		&& PeCoHUD->GetPlayerOverlayWidget()->HealthBar->ProgressBar;
	if (bHUDValid)
	{
		const float HealthPercent = Health / MaxHealth;
		PeCoHUD->GetPlayerOverlayWidget()->HealthBar->ProgressBar->SetPercent(HealthPercent);
		FString HealthText = FString::Printf(TEXT("%d/%d"), FMath::CeilToInt(Health), FMath::CeilToInt(MaxHealth));
		PeCoHUD->GetPlayerOverlayWidget()->HealthBar->Text->SetText(FText::FromString(HealthText));
	}
}
void APeCoPlayerController::SetHUDExpBar(float Exp, float MaxExp)
{
	PeCoHUD = PeCoHUD == nullptr ? Cast<APeCoHUD>(GetHUD()) : PeCoHUD;
	bool bHUDValid = PeCoHUD
		&& PeCoHUD->GetPlayerOverlayWidget()
		&& PeCoHUD->GetPlayerOverlayWidget()->ExpBar
		&& PeCoHUD->GetPlayerOverlayWidget()->ExpBar->ProgressBar;
	if (bHUDValid)
	{
		const float HealthPercent = Exp / MaxExp;
		PeCoHUD->GetPlayerOverlayWidget()->HealthBar->ProgressBar->SetPercent(HealthPercent);
		FString HealthText = FString::Printf(TEXT("%d/%d"), FMath::CeilToInt(Exp), FMath::CeilToInt(MaxExp));
		PeCoHUD->GetPlayerOverlayWidget()->HealthBar->Text->SetText(FText::FromString(HealthText));
	}
}

void APeCoPlayerController::GetGameTimeData()
{
	PeCoGameMode = PeCoGameMode == nullptr ? Cast<APeCoGameMode>(UGameplayStatics::GetGameMode(this)) : PeCoGameMode;
	if (PeCoGameMode)
	{
		TotalGameTime = PeCoGameMode->TotalGameTime;
		// To Do : 다른 시간 변수들 설정..
	}
}
void APeCoPlayerController::SetHUDTime(float DeltaTime)
{
	float TimeLeft = 0.f;

	PeCoGameMode = PeCoGameMode == nullptr ? Cast<APeCoGameMode>(UGameplayStatics::GetGameMode(this)) : PeCoGameMode;
	if (PeCoGameMode)
	{
		LevelStartingTime = PeCoGameMode->LevelStartingTime;
	}
	TimeLeft = LevelStartingTime + TotalGameTime - GetWorld()->GetTimeSeconds();
	uint32 SecondsLeft = FMath::CeilToInt(TimeLeft);

	if (CountdownInt != SecondsLeft) // Every One Seconds Later
	{
		SetHUDGameTimer(TimeLeft);
	}
	CountdownInt = SecondsLeft;

}
void APeCoPlayerController::SetHUDGameTimer(float CountdownTime)
{
	PeCoHUD = PeCoHUD == nullptr ? Cast<APeCoHUD>(GetHUD()) : PeCoHUD;
	bool bHUDValid = PeCoHUD 
		&& PeCoHUD->GetPlayerOverlayWidget()
		&& PeCoHUD->GetPlayerOverlayWidget()->GameTimer
		&& PeCoHUD->GetPlayerOverlayWidget()->GameTimer->GameTimer;
	if (bHUDValid)
	{
		if (CountdownTime < 0.f) // While Transition, it set as negative value
		{
			PeCoHUD->GetPlayerOverlayWidget()->GameTimer->GameTimer->SetText((FText()));
			return;
		}
		int32 Minutes = FMath::FloorToInt(CountdownTime / 60.f);
		int32 Seconds = CountdownTime - Minutes * 60;
		FString CountdownText = FString::Printf(TEXT("%02d : %02d"), Minutes, Seconds);
		PeCoHUD->GetPlayerOverlayWidget()->GameTimer->GameTimer->SetText(FText::FromString(CountdownText));
	}
}


