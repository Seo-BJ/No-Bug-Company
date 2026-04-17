// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "PoolableInterface.generated.h"

UINTERFACE(MinimalAPI)
class UPoolable : public UInterface
{
	GENERATED_BODY()
};

/**
 * 오브젝트 풀에서 재사용 가능한 액터가 구현해야 하는 인터페이스.
 * - OnAcquired : 풀에서 꺼내져 활성화될 때. 상태를 초기화하고 활성 준비.
 * - OnReleased : 풀로 반환되기 직전. 상태를 정리하고 비활성화.
 */
class PESTCONTROL_API IPoolable
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintNativeEvent, Category = "Pool")
	void OnAcquired(const FTransform& SpawnTransform, AActor* NewOwner, APawn* NewInstigator);
	
	virtual void OnAcquired_Implementation(const FTransform& SpawnTransform, AActor* NewOwner, APawn* NewInstigator) {}

	UFUNCTION(BlueprintNativeEvent, Category = "Pool")
	void OnReleased();
	
	virtual void OnReleased_Implementation() {}
};
