// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "PeCoCharacter.generated.h"

UCLASS()
class PESTCONTROL_API APeCoCharacter : public ACharacter
{
	GENERATED_BODY()

public:

	APeCoCharacter();

	void Fire();

protected:

	// ~ Player Character Interface
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	void RotateAim(FVector LookAtTarget);

private:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	USceneComponent* ProjectileSpawnPoint;

	UPROPERTY(EditDefaultsOnly, Category = "Combat")
	TSubclassOf<class AProjectile> ProjectileClass;
};
