// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "PeCoPlayerState.generated.h"

class UDataTable;
/**
 * 
 */
UCLASS()
class PESTCONTROL_API APeCoPlayerState : public APlayerState
{
	GENERATED_BODY()



public:

	APeCoPlayerState();

	// ~ Player Stats
	UPROPERTY(EditAnywhere, Category = "Player Stats")
	float Health;

	UPROPERTY(EditAnywhere, Category = "Player Stats")
	float MaxHealth = 100;
		

	UFUNCTION(BlueprintCallable)
	void AddToKillCount(int32 KillCountAmount);

	UFUNCTION(BlueprintCallable)
	int32 GetKillCount();

	UFUNCTION(BlueprintCallable)
	void SetKillCount(int32 KillCountAmount);

	UPROPERTY()
	int32 CurrentLevel = 1;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "LevelUp", meta = (AllowPrivateAccess = "true"))
	UDataTable* LevelUpDataTable; 
	// 레벨업을 처리하는 함수
	UFUNCTION(BlueprintCallable)
	void CheckLevelUp();


	// 레벨업 시 실행되는 함수
	void HandleLevelUp(int32 NewLevel);



protected:

private:
	UPROPERTY()
	int32 KillCount = 0;
	
};
