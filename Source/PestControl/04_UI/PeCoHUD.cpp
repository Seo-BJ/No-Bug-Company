// Fill out your copyright notice in the Description page of Project Settings.


#include "PeCoHUD.h"

#include "02_Player/PeCoPlayerController.h"

#include "04_UI/PeCoUserWidget.h"
#include "04_UI/PlayerOverlay.h"

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

}
