// Fill out your copyright notice in the Description page of Project Settings.


#include "PeCoPlayerController.h"



#include "03_Input/InPutActionDataAsset.h"


#include "Components/InputComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputMappingContext.h"


#include "GameFramework/Pawn.h"
#include "Kismet/GameplayStatics.h"

#include "Blueprint/UserWidget.h"



void APeCoPlayerController::BeginPlay()
{
	Super::BeginPlay();

	APlayerController* PlayerController = Cast<APlayerController>(this);
	if (PlayerController != nullptr)
	{
		UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(
			PlayerController->GetLocalPlayer());
		if (Subsystem != nullptr)
		{
			Subsystem->AddMappingContext(InputMappingContextMove, 0);
		}
	}
}

void APeCoPlayerController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
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
