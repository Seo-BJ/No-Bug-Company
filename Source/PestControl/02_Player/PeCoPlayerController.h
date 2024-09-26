// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "InputActionValue.h"
#include "PeCoPlayerController.generated.h"

class UUserWidget;
class UInputMappingContext;
class UInPutActionDataAsset;

UCLASS()
class PESTCONTROL_API APeCoPlayerController : public APlayerController
{
	GENERATED_BODY()
	
	// ~Player Movement
public:
	void Move(const FInputActionValue& Value);

	virtual void Tick(float DeltaTime) override;

protected:

	virtual void BeginPlay() override;
	
	virtual void SetupInputComponent() override;

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
};
