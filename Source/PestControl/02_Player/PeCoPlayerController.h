// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "InputActionValue.h"
#include "PeCoPlayerController.generated.h"


class UInputMappingContext;

class UInPutActionDataAsset;

UCLASS()
class PESTCONTROL_API APeCoPlayerController : public APlayerController
{
	GENERATED_BODY()
	
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

};
