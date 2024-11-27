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
	void ActivateItem(AActor* User);

	void ActivateInvincibility(float Duration, AActor* User);
	void DeactivateInvincibility();

	FTimerHandle InvincibilityTimerHandle;

protected:

	virtual void BeginPlay() override;

public:	

	virtual void Tick(float DeltaTime) override;

private:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	USceneComponent* Root;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* Mesh;

private:
	AActor* AffectedUser;

};
