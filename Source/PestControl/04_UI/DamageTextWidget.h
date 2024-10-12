// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "04_UI/PeCoUserWidget.h"
#include "DamageTextWidget.generated.h"

/**
 * 
 */
UCLASS()
class PESTCONTROL_API UDamageTextWidget : public UPeCoUserWidget
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable)
    void SetDamageText(float Damage);
    void UpdatePosition(FVector EnemyWorldPosition, APlayerController* PlayerController);

protected:
    UPROPERTY(meta = (BindWidget))
    class UTextBlock* DamageTextBlock;
};