// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "04_UI/PeCoUserWidget.h"
#include "SpecialEventWidget.generated.h"

/**
 * 
 */
UCLASS()
class PESTCONTROL_API USpecialEventWidget : public UPeCoUserWidget
{
    GENERATED_BODY()

protected:
    virtual void NativeConstruct() override;

public:
    // Set the event name (title)
    UFUNCTION(BlueprintCallable, Category = "Event")
    void SetEventName(const FString& EventName);

    // Set the event description
    UFUNCTION(BlueprintCallable, Category = "Event")
    void SetEventDescription(const FString& EventDescription);

    // TextBlock for the event title (Flood)
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (BindWidget))
    class UTextBlock* EventNameText;

    // TextBlock for the event description
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (BindWidget))
    class UTextBlock* EventDescriptionText;

};