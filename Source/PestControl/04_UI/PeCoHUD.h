// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "PeCoHUD.generated.h"

class UPlayerOverlay;
/**
 * 
 */
UCLASS()
class PESTCONTROL_API APeCoHUD : public AHUD
{
	GENERATED_BODY()
	
public :

	virtual void DrawHUD() override;

	void InitOverlay(APlayerController* PC, APlayerState* PS);



protected:


private:

public : 

	UPROPERTY()
	TObjectPtr<UPlayerOverlay>  PlayerOverlayWidget;
	UPROPERTY(EditAnywhere)
	TSubclassOf<UPlayerOverlay> PlayerOverlayWidgetClass;

};
