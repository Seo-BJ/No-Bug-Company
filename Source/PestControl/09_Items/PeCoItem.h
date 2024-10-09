// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "21_Data/PeCoDataTypes.h"

#include "PeCoItem.generated.h"

UCLASS()
class PESTCONTROL_API APeCoItem : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	APeCoItem();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item Properties")
	EItemType ItemType = EItemType::EItemType_Default;

	// Icon image for the item
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item Properties")
	UTexture2D* ItemIcon;

	// Name of the item
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item Properties")
	FString ItemName;

	// Description of the item
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item Properties")
	FString ItemDescription;


	virtual void UseItem();



	FORCEINLINE EItemType GetItemType() { return ItemType; }
	

};
