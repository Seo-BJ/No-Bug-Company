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
}


void APeCoPlayerController::ShowLevelUpUI()
{
	
	/*
	if (LevelUpWidgetClass && !LevelUpWidget)
	{

		
		// 레벨업 UI 위젯을 생성하고 화면에 표시
		LevelUpWidget = CreateWidget<UUserWidget>(this, LevelUpWidgetClass);
		if (LevelUpWidget)
		{
			LevelUpWidget->AddToViewport();
			// 마우스 커서 표시 (UI 상호작용을 위해)
			bShowMouseCursor = true;
			SetInputMode(FInputModeUIOnly());
		}
		
		
	
	}
	
	*/


}

void APeCoPlayerController::CloseLevelUpUI()
{
	/*
	if (LevelUpWidget)
	{
		
		LevelUpWidget->RemoveFromViewport();
		LevelUpWidget = nullptr;

		// 마우스 커서 숨기고 게임으로 돌아감
		bShowMouseCursor = false;
		SetInputMode(FInputModeGameOnly());

		// 게임 재개
		SetPause(false);
		
		
		
		
	}
	
	*/
}

void APeCoPlayerController::SetHUDHealthBar(float Health, float MaxHealth)
{
	PeCoHUD = PeCoHUD == nullptr ? Cast<APeCoHUD>(GetHUD()) : PeCoHUD;
	bool bHUDValid = PeCoHUD
		&& PeCoHUD->PlayerOverlayWidget
		&& PeCoHUD->PlayerOverlayWidget->HealthBar
		&& PeCoHUD->PlayerOverlayWidget->HealthBar->ProgressBar;
	if (bHUDValid)
	{
		const float HealthPercent = Health / MaxHealth;
		PeCoHUD->PlayerOverlayWidget->HealthBar->ProgressBar->SetPercent(HealthPercent);
		FString HealthText = FString::Printf(TEXT("%d/%d"), FMath::CeilToInt(Health), FMath::CeilToInt(MaxHealth));
		PeCoHUD->PlayerOverlayWidget->HealthBar->Text->SetText(FText::FromString(HealthText));
	}
}

void APeCoPlayerController::SetHUDExpBar(float Exp, float MaxExp)
{
	PeCoHUD = PeCoHUD == nullptr ? Cast<APeCoHUD>(GetHUD()) : PeCoHUD;
	bool bHUDValid = PeCoHUD
		&& PeCoHUD->PlayerOverlayWidget
		&& PeCoHUD->PlayerOverlayWidget->ExpBar
		&& PeCoHUD->PlayerOverlayWidget->ExpBar->ProgressBar;
	if (bHUDValid)
	{
		const float HealthPercent = Exp / MaxExp;
		PeCoHUD->PlayerOverlayWidget->HealthBar->ProgressBar->SetPercent(HealthPercent);
		FString HealthText = FString::Printf(TEXT("%d/%d"), FMath::CeilToInt(Exp), FMath::CeilToInt(MaxExp));
		PeCoHUD->PlayerOverlayWidget->HealthBar->Text->SetText(FText::FromString(HealthText));
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
		&& PeCoHUD->PlayerOverlayWidget 
		&& PeCoHUD->PlayerOverlayWidget->GameTimer
		&& PeCoHUD->PlayerOverlayWidget->GameTimer->GameTimer;
	if (bHUDValid)
	{
		if (CountdownTime < 0.f) // While Transition, it set as negative value
		{
			PeCoHUD->PlayerOverlayWidget->GameTimer->GameTimer->SetText((FText()));
			return;
		}
		int32 Minutes = FMath::FloorToInt(CountdownTime / 60.f);
		int32 Seconds = CountdownTime - Minutes * 60;
		FString CountdownText = FString::Printf(TEXT("%02d : %02d"), Minutes, Seconds);
		PeCoHUD->PlayerOverlayWidget->GameTimer->GameTimer->SetText(FText::FromString(CountdownText));
	}
}

void APeCoPlayerController::Move(const FInputActionValue& Value)
{
	FVector2D MovementVector = Value.Get<FVector2D>();

	const FRotator	Rotation = GetControlRotation();
	const FRotator	YawRotation(0, Rotation.Yaw, 0);

	const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	
	const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

	GetPawn()->AddMovementInput(ForwardDirection, MovementVector.Y);
	GetPawn()->AddMovementInput(RightDirection, MovementVector.X);	
}
