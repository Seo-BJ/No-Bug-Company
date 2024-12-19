// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "VentPoint.generated.h"

UCLASS()
class PESTCONTROL_API AVentPoint : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AVentPoint();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;



public:	
	
	// 벤트 고유 번호
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Vent")
	int32 VentNumber;

	// 벤트의 위치 반환
	UFUNCTION(BlueprintCallable, Category = "Vent")
	FVector GetVentLocation() const;

private:
	FVector VentLocation;

};
