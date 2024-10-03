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

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	USceneComponent* ProjectileSpawnPoint;

protected:

	// ~ Player Character Interface
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	void RotateAim(FVector LookAtTarget);

private:

	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	TSubclassOf<class AProjectile> ProjectileClass;
};
