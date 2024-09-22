// Fill out your copyright notice in the Description page of Project Settings.


#include "PeCoPlayerController.h"

#include "Components/InputComponent.h"
#include "GameFramework/Pawn.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputMappingContext.h"
#include "03_Input/InPutActionDataAsset.h"
#include "Kismet/GameplayStatics.h"


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
