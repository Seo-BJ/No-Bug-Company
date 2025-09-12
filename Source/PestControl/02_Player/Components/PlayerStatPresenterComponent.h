// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PeCoGameplayTags.h"

#include "PlayerStatPresenterComponent.generated.h"

class APeCoPlayerState;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FPresenterStatChanged, float, OldValue, float, NewValue);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PESTCONTROL_API UPlayerStatPresenterComponent : public UActorComponent
{
	GENERATED_BODY()

public:	

	UPlayerStatPresenterComponent();

	void BroadcastInitialValues();
	void BindCallbacksToDependencies() const;

protected:

	virtual void BeginPlay() override;

public:	

	UFUNCTION(BlueprintCallable)
	void AddHealth(float Amount, AActor* CauserActor = nullptr);
	UFUNCTION(BlueprintCallable)
	void MaximizeHealth(AActor* CauserActor = nullptr);
	UFUNCTION(BlueprintCallable)
	void MultiplyHealth(float Percent, AActor* CauserActor = nullptr);

	UFUNCTION(BlueprintCallable, Category = "BuffSystem|Speed")
	void BuffSpeed(float Percent, float BuffTime);
	UFUNCTION(BlueprintCallable, Category = "BuffSystem|Speed")
	void ResetSpeeds();

	UFUNCTION(BlueprintCallable)
	void UpgradeStat(FGameplayTag StatTag);

	UFUNCTION(BlueprintCallable, Category = "BuffSystem|WeaponStat")
	void BuffWeaponStat(FGameplayTag StatTag, float Amount, float BuffTime , float Percent);
	UFUNCTION(BlueprintCallable, Category = "BuffSystem|WeaponStat")
	void ResetWeaponStat(FGameplayTag StatTag);

	FTimerHandle SpeedBuffTimer;

	float InitialBaseSpeed = 0.f;
	float InitialCrouchSpeed = 0.f;

	TMap<FGameplayTag, float> PreviousWeaponStats;

	UPROPERTY(BlueprintAssignable, Category = "Player|Stat|Event")
	FPresenterStatChanged OnHealthChanged;

	UPROPERTY(BlueprintAssignable, Category = "Player|Stat|Event")
	FPresenterStatChanged OnMaxHealthChanged;

	UPROPERTY(BlueprintAssignable, Category = "Player|Stat|Event")
	FPresenterStatChanged OnMoveSpeedChanged;

	UPROPERTY(BlueprintAssignable, Category = "Player|Stat|Event")
	FPresenterStatChanged OnDamageResistanceChanged;

	UPROPERTY(BlueprintAssignable, Category = "Player|Stat|Event")
	FPresenterStatChanged OnSkillCoolTimeChanged;

private:

		
};
