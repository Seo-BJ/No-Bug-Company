// Fill out your copyright notice in the Description page of Project Settings.


#include "04_UI/PeCoUserWidget.h"

#include "02_Player/PeCoPlayerState.h"
#include "02_Player/Components/PlayerLevelPresenterComponent.h"
#include "02_Player/Components/PlayerStatPresenterComponent.h"

UPlayerStatPresenterComponent* UPeCoUserWidget::GetStatPresenter()
{
	APeCoPlayerState* PlayerState = GetOwningPlayerState<APeCoPlayerState>();
	if (IsValid(PlayerState))
	{
		UPlayerStatPresenterComponent* StatPresenter = PlayerState->FindComponentByClass<UPlayerStatPresenterComponent>();
		if (IsValid(StatPresenter))
		{
			return StatPresenter;
		}
	}
	return nullptr;
}
