// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "01_Character/PeCoCharacter.h"
#include "GameFramework/Character.h"
#include "PeCoEnemyCharacter.generated.h"

/**
 * 
 */
UCLASS()
class PESTCONTROL_API APeCoEnemyCharacter : public APeCoCharacter
{
	GENERATED_BODY()

public: 
	APeCoEnemyCharacter();

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Enemy")
	float MovementSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy")
	class UBehaviorTree* BehaviorTree;
	
};
