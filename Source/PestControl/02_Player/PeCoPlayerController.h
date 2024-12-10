// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "InputActionValue.h"
#include "PeCoGameplayTags.h"
#include "21_Data/PeCoDataTypes.h"


#include "PeCoPlayerController.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FStartDash);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FStartDashCooldown, float, DashCooldown);

class UUserWidget;
class UInputMappingContext;
class UInPutActionDataAsset;
class UDamageTextComponent;
class APeCoHUD;
class APeCoCharacter;
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

	// void SetHUDItemSlotCount(EConsumableItemType ItemType, uint32 Amount);


	UPROPERTY(BlueprintAssignable)
	FStartDash OnStartDash;
	UPROPERTY(BlueprintAssignable)
	FStartDashCooldown OnStartDashCooldown;

	//~Floating Widgets
	void ShowDamageTextWidget(float DamageAmount, APeCoCharacter* TargetCharacter, bool bCriticalHit, bool bBlockedHit);
	void ShowSupplyResultWidget(TMap<FGameplayTag, int32> SupplyResultMap);
	//~End of Floating Widgets

protected:

	//~Input Actions
	void Move(const FInputActionValue& Value);
	void Dash(const FInputActionValue& Value);


	void PressConsumableItemKey(const FInputActionValue& Value);
	void HeldConsumableItemKey(const FInputActionValue& Value);

	void PressCombatbleItemKey(const FInputActionValue& Value);
	void HeldCombatItemKey(const FInputActionValue& Value);
	
	void UseConsumableItem(const FInputActionValue& Value);
	void UseCombatleItem(const FInputActionValue& Value);

	bool bConsumableItemKeyPressed = false;
	bool bCombatItemKeyPressed = false;
	//~End of Input Actions

	void ResetDash();
	void CoolDownDash();

	FVector2D CurrentMoveDirection;


	//~Timer Widget
	float TotalGameTime = 0.f;
	float LevelStartingTime = 0.f;

	uint32 CountdownInt = 0;
	// To do : 라운드 별 시간 추가?

	void GetGameTimeData();
	void SetHUDTime(float DeltaTime);
	void SetHUDGameTimer(float CountdownTime);
	//~End of Timer Widget



private:

	UPROPERTY(EditAnywhere, Category = Input)
	UInputMappingContext* InputMappingContextMove;

	UPROPERTY(EditAnywhere, Category = Input)
	UInPutActionDataAsset* InputActions;

	UPROPERTY(EditAnywhere, Category = "Dash")
	float DashCooldown = 2.0f;

	UPROPERTY(EditAnywhere, Category = "Dash")
	float DashDuration = 0.4f;

	bool bCanDash = true;
	FTimerHandle DashTimer;
	FVector DashDistance;

	UPROPERTY(EditAnywhere, Category = "Dash")
	bool bIsDashing = false;

	// ~End of Player Movement

	// Class Reference
	TObjectPtr<APeCoHUD> PeCoHUD = nullptr;

	TObjectPtr<APeCoGameMode> PeCoGameMode = nullptr;






};
