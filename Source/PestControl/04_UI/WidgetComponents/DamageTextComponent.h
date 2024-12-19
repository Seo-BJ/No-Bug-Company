// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/WidgetComponent.h"
#include "DamageTextComponent.generated.h"

/**
 * 
 */
UCLASS()
class PESTCONTROL_API UDamageTextComponent : public UWidgetComponent
{
	GENERATED_BODY()

public:
	// ToDo : Damage 속성 별로 색상을 다르게 설정?
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void SetDamageText(float Damage, bool bCriticalHit, bool bBlockedHit, bool bPlayerGetDamage);
};
