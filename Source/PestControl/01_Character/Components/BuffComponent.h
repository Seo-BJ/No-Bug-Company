// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "BuffComponent.generated.h"

class APeCoPlayerState;
class APeCoPlayerCharacter;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PESTCONTROL_API UBuffComponent : public UActorComponent
{
	GENERATED_BODY()

public:	

	UBuffComponent();
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;


	//~Health

	UFUNCTION(BlueprintCallable, Category = "BuffSystem|Health")
	void Heal(float HealAmount);
	UFUNCTION(BlueprintCallable, Category = "BuffSystem|Health")
	void HealToMaxHealth();
	UFUNCTION(BlueprintCallable, Category = "BuffSystem|Health")
	void HealByPercent(float Percent);

	//~End of Health

	//~Speed Buff

	UFUNCTION(BlueprintCallable, Category = "BuffSystem|Speed")
	void BuffSpeed(float BuffBaseSpeed, float BuffCrouchSpeed, float BuffTime);
	UFUNCTION(BlueprintCallable, Category = "BuffSystem|Speed")
	void ResetSpeeds();
	UFUNCTION(BlueprintCallable, Category = "BuffSystem|Speed")
	void SetInitialSpeeds(float BaseSpeed, float CrouchSpeed);

	// Speed Buff
	FTimerHandle SpeedBuffTimer;

	//~End of Speed Buff

	float InitialBaseSpeed = 0.f;
	float InitialCrouchSpeed = 0.f;

protected:

	virtual void BeginPlay() override;


private:

	APeCoPlayerState* GetPlayerState();
	APeCoPlayerCharacter* GetPlayerCharacter();

};
