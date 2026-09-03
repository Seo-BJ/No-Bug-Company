// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "21_Data/PeCoDataTypes.h"
#include "PeCoCharacter.generated.h"

UCLASS()
class PESTCONTROL_API APeCoCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	APeCoCharacter(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
	
	virtual ETeam GetTeam();
	
protected:
	//~ AActor Interface
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	//~End of AActor Interface


};
