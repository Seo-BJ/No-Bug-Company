// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AntiSpray.generated.h"

UCLASS()
class PESTCONTROL_API AAntiSpray : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AAntiSpray();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	float InvincibilityDuration = 5.0f;

	UFUNCTION(BlueprintCallable, Category = "Item")
	void ActivateItem(class APeCoPlayerCharacter* Character);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	USceneComponent* Root;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* Mesh;

};
