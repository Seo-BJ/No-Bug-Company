// Fill out your copyright notice in the Description page of Project Settings.


#include "04_UI/SubWidget/SpecialEventWidget.h"
#include "Components/TextBlock.h"

void USpecialEventWidget::NativeConstruct()
{
    Super::NativeConstruct();
}

void USpecialEventWidget::SetEventName(const FString& EventName)
{
    if (EventNameText)
    {
        EventNameText->SetText(FText::FromString(EventName));
    }
}

void USpecialEventWidget::SetEventDescription(const FString& EventDescription)
{
    if (EventDescriptionText)
    {
        EventDescriptionText->SetText(FText::FromString(EventDescription));
    }
}
