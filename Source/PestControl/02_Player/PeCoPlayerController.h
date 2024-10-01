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

protected:


	void Move(const FInputActionValue& Value);

	
	

private:

	UPROPERTY(EditAnywhere, Category = Input)
	UInputMappingContext* InputMappingContextMove;

	UPROPERTY(EditAnywhere, Category = Input)
	UInPutActionDataAsset* InputActions;

	// ~End of Player Movement

	// ~Player Level Up
public:

	// 레벨업 UI를 표시하는 함수
	UFUNCTION(BlueprintCallable)
	void ShowLevelUpUI();

	// UI를 숨기고 일시정지를 해제하는 함수
	UFUNCTION(BlueprintCallable)
	void CloseLevelUpUI();

private:

	// 레벨업 보상 UI 위젯 클래스
	// UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI", meta = (AllowPrivateAccess = "true"))
	//	TSubclassOf<UUserWidget> LevelUpWidgetClass;

	// 현재 표시 중인 레벨업 UI 위젯 인스턴스
	// UPROPERTY()
	// UUserWidget* LevelUpWidget;


	// ~End of Player Level Up 

#pragma region Health, Exp Widget

public:

	void SetHUDHealthBar(float Health, float MaxHealth);
	void SetHUDExpBar(float Exp, float MaxExp);

protected:


private:

#pragma endregion
#pragma region GameTime & TimerWidget

public:

protected:

	// ~ Region Timer Widget
	float TotalGameTime = 0.f;
	float LevelStartingTime = 0.f;

	// To do : 라운드 별 시간 추가?

	void GetGameTimeData();
	void SetHUDTime(float DeltaTime);
	void SetHUDGameTimer(float CountdownTime);
	// ~ End Region Timer Widget

private:

	uint32 CountdownInt = 0;

#pragma endregion

#pragma region Refrence Pointer
private:
	// HUD Class Reference
	TObjectPtr<APeCoHUD> PeCoHUD = nullptr;

	TObjectPtr<APeCoGameMode> PeCoGameMode = nullptr;

#pragma endregion
};
