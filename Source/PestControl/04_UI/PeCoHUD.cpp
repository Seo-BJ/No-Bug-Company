// Fill out your copyright notice in the Description page of Project Settings.


#include "PeCoHUD.h"

#include "02_Player/PeCoPlayerController.h"

#include "04_UI/PeCoUserWidget.h"
#include "04_UI/PlayerOverlay.h"
#include "04_UI/DamageTextWidget.h"
#include "04_UI/SubWidget/SpecialEventWidget.h"

#include "Kismet/GameplayStatics.h"

void APeCoHUD::DrawHUD()
{
	Super::DrawHUD();
}

void APeCoHUD::InitOverlay(APlayerController* PC, APlayerState* PS)
{
	checkf(PlayerOverlayWidgetClass, TEXT("Overlay Widget Class가 초기화 되지 않음. BP_PeCOHUD 에서 Overlay Widget Class를 설정해야 합니다."));

	APeCoPlayerController* PeCoPC = Cast< APeCoPlayerController>(GetOwningPlayerController());
	checkf(PeCoPC, TEXT("PeCo Player Controller 가져오는 것을 실패했습니다."));

	PlayerOverlayWidget = CreateWidget<UPlayerOverlay>(PeCoPC, PlayerOverlayWidgetClass);
	PlayerOverlayWidget->AddToViewport();

	// PlayerOverlayWidget->GetWidgetFromName

}

void APeCoHUD::AddLevelUpWidget()
{
	OwningPlayerController = OwningPlayerController == nullptr ? GetOwningPlayerController() : OwningPlayerController;
	if (OwningPlayerController && LevelUpWidgetClass)
	{
		LevelUpWidget = CreateWidget<UPeCoUserWidget>(OwningPlayerController, LevelUpWidgetClass);
		LevelUpWidget->AddToViewport();
	}
}
void APeCoHUD::AddGameOverWidget()
{
	OwningPlayerController = OwningPlayerController == nullptr ? GetOwningPlayerController() : OwningPlayerController;
	if (OwningPlayerController && GameOverWidgetClass)
	{
		GameOverWidget = CreateWidget<UPeCoUserWidget>(OwningPlayerController, GameOverWidgetClass);
		GameOverWidget->AddToViewport();
	}
}
void APeCoHUD::AddGameResultWidget()
{
	OwningPlayerController = OwningPlayerController == nullptr ? GetOwningPlayerController() : OwningPlayerController;
	if (OwningPlayerController && GameResultWidgetClass)
	{
		GameResultWidget = CreateWidget<UPeCoUserWidget>(OwningPlayerController, GameResultWidgetClass);
		GameResultWidget->AddToViewport();
	}
}
void APeCoHUD::AddDamageTextToPlayer(float Damage, FVector2D ScreenPosition)
{
    if (DamageTextWidgetClass)
    {
		OwningPlayerController = OwningPlayerController == nullptr ? GetOwningPlayerController() : OwningPlayerController;
        if (OwningPlayerController)
        {
            UDamageTextWidget* DamageWidget = CreateWidget<UDamageTextWidget>(OwningPlayerController, DamageTextWidgetClass);
            if (DamageWidget)
            {
                DamageWidget->SetDamageText(Damage);
                DamageWidget->AddToViewport();
				DamageWidget->SetPositionInViewport(ScreenPosition, false);

                FTimerHandle TimerHandle;
                GetWorld()->GetTimerManager().SetTimer(TimerHandle, [DamageWidget]()
                    {
                        DamageWidget->RemoveFromParent();
                    }, 2.0f, false); 
            }
        }
    }
}