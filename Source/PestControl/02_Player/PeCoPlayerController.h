// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "InputActionValue.h"


#include "PeCoPlayerController.generated.h"


class UUserWidget;
class UInputMappingContext;
class UInPutActionDataAsset;

class APeCoHUD;
class APeCoGameMode;

UCLASS()
class PESTCONTROL_API APeCoPlayerController : public APlayerController
{
	GENERATED_BODY()
	
	// ~Player Movement
public:
	APeCoPlayerController();

	//~AActor interface
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void OnPossess(APawn* InPawn) override;
	virtual void SetupInputComponent() override;
	//~End of AActor interface

	void SetHUDHealthBar(float Health, float MaxHealth);
	void SetHUDExpBar(float Exp, float MaxExp);



protected:

	void Move(const FInputActionValue& Value);
	void Dash(const FInputActionValue& Value);
	void ResetDash();

	FVector2D CurrentMoveDirection;


	// ~ Region Timer Widget
	float TotalGameTime = 0.f;
	float LevelStartingTime = 0.f;

	// To do : 라운드 별 시간 추가?

	void GetGameTimeData();
	void SetHUDTime(float DeltaTime);
	void SetHUDGameTimer(float CountdownTime);

	// ~ End Region Timer Widget

	uint32 CountdownInt = 0;


private:

	UPROPERTY(EditAnywhere, Category = Input)
	UInputMappingContext* InputMappingContextMove;

	UPROPERTY(EditAnywhere, Category = Input)
	UInPutActionDataAsset* InputActions;

	UPROPERTY(EditAnywhere, Category = "Dash")
	float DashDistance = 600.0f;

	UPROPERTY(EditAnywhere, Category = "Dash")
	float DashCooldown = 2.0f;

	FTimerHandle DashTimer;

	bool bCanDash = true;
	// ~End of Player Movement

		// HUD Class Reference
	TObjectPtr<APeCoHUD> PeCoHUD = nullptr;

	TObjectPtr<APeCoGameMode> PeCoGameMode = nullptr;






};
