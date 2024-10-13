// Fill out your copyright notice in the Description page of Project Settings.


#include "04_UI/DamageTextWidget.h"
#include "Components/TextBlock.h"

#include "Kismet/GameplayStatics.h"

#include "Engine/Engine.h"  

void UDamageTextWidget::SetDamageText(float Damage)
{
    if (DamageTextBlock)
    {
        DamageTextBlock->SetText(FText::AsNumber(FMath::RoundToInt(Damage)));
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("DamageTextBlock is NULL!"));
    }
}

void UDamageTextWidget::UpdatePosition(FVector EnemyWorldPosition, APlayerController* PlayerController)
{
    FVector2D ScreenPosition;
    if (UGameplayStatics::ProjectWorldToScreen(PlayerController, EnemyWorldPosition, ScreenPosition))
    {
        SetPositionInViewport(ScreenPosition, false);
    }
}