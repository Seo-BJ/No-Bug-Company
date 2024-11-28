// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PeCoGameplayTags.h"

#include "StoreComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PESTCONTROL_API UStoreComponent : public UActorComponent
{
	GENERATED_BODY()

public:	

	UStoreComponent();

protected:

	virtual void BeginPlay() override;

public:	


	UFUNCTION(BlueprintCallable)
	FGameplayTagContainer GetRandomStatTags(int32 count);

	UFUNCTION(BlueprintCallable)
	float  GetStatUpgradeData(FGameplayTag StatTag, APlayerController* PlayerController);
		
};
