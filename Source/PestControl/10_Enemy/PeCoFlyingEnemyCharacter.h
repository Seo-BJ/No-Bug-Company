// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "01_Character/PeCoEnemyCharacter.h"
#include "PeCoFlyingEnemyCharacter.generated.h"

/**
 * 
 */
UCLASS()
class PESTCONTROL_API APeCoFlyingEnemyCharacter : public APeCoEnemyCharacter
{
	GENERATED_BODY()
	
public:
    APeCoFlyingEnemyCharacter();
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flying")
    float FlyingSpeed = 400.f; // Set FlyingSpeed

protected:
    virtual void BeginPlay() override;
    virtual void Tick(float DeltaTime) override;

    


};
