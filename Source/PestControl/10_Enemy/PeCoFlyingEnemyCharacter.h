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

public:
    // 풀에서 재사용 시 비행 모드 강제 복구. (BeginPlay가 재호출되지 않기 때문)
    virtual void OnAcquired_Implementation(const FTransform& SpawnTransform, AActor* NewOwner, APawn* NewInstigator) override;
};
