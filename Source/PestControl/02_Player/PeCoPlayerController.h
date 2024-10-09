// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "InputActionValue.h"
#include "21_Data/PeCoDataTypes.h"


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
	void SetHUDItemSlotCount(EItemType ItemType, uint32 Amount);



protected:

	void Move(const FInputActionValue& Value);
	void Dash(const FInputActionValue& Value);
	void ResetDash();
	void CoolDownDash();

	FVector2D CurrentMoveDirection;


	//~Timer Widget
	float TotalGameTime = 0.f;
	float LevelStartingTime = 0.f;

	// To do : 라운드 별 시간 추가?

	void GetGameTimeData();
	void SetHUDTime(float DeltaTime);
	void SetHUDGameTimer(float CountdownTime);

	//~End of Timer Widget

	uint32 CountdownInt = 0;


private:

	UPROPERTY(EditAnywhere, Category = Input)
	UInputMappingContext* InputMappingContextMove;

	UPROPERTY(EditAnywhere, Category = Input)
	UInPutActionDataAsset* InputActions;

	UPROPERTY(EditAnywhere, Category = "Dash")
	float DashVelocity = 4000.0f;

	UPROPERTY(EditAnywhere, Category = "Dash")
	float DashCooldown = 2.0f;

	UPROPERTY(EditAnywhere, Category = "Dash")
	float DashDuration = 0.4f;

	bool bCanDash = true;
	FTimerHandle DashTimer;
	FVector DashDistance;

	// ~End of Player Movement

		// HUD Class Reference
	TObjectPtr<APeCoHUD> PeCoHUD = nullptr;

	TObjectPtr<APeCoGameMode> PeCoGameMode = nullptr;






};
