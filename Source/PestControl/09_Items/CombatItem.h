// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CombatItem.generated.h"

UCLASS()
class PESTCONTROL_API ACombatItem : public AActor
{
	GENERATED_BODY()
	
public:	

	ACombatItem();

protected:

	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable)
	virtual void UseCombatItem();

};
