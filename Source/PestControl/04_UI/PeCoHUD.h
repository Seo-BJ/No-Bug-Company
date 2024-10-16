// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "PeCoHUD.generated.h"

class UPlayerOverlay;
class UPeCoUserWidget;
class UDamageTextWidget;
class APlayerController;
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

	void AddLevelUpWidget();
	void AddGameOverWidget();
	UFUNCTION(BlueprintCallable)
	void AddGameResultWidget();

	void AddDamageTextToPlayer(float Damage, FVector2D ScreenPosition);

private:

	TObjectPtr<UPeCoUserWidget>  LevelUpWidget;
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UPeCoUserWidget> LevelUpWidgetClass;

	TObjectPtr<UPeCoUserWidget>  GameOverWidget;
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UPeCoUserWidget>  GameOverWidgetClass;


	TObjectPtr<UPeCoUserWidget>  GameResultWidget;
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UPeCoUserWidget>  GameResultWidgetClass;


	TObjectPtr<UPlayerOverlay>  PlayerOverlayWidget;
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UPlayerOverlay> PlayerOverlayWidgetClass;

	TObjectPtr<APlayerController> OwningPlayerController;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UDamageTextWidget> DamageTextWidgetClass; 

public:
	
	FORCEINLINE UPlayerOverlay* GetPlayerOverlayWidget() { return PlayerOverlayWidget; }

};
 