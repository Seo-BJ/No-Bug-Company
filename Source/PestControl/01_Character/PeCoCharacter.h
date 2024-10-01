// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "PeCoDataTypes.h"
#include "PeCoCharacter.generated.h"

UCLASS()
class PESTCONTROL_API APeCoCharacter : public ACharacter
{
	GENERATED_BODY()

public:

	APeCoCharacter();

	//~ AActor Interface

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	//~End of AActor Interface


	virtual ETeam GetTeam();

	void Fire();

protected:



	void RotateAim(FVector LookAtTarget);

private:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	USceneComponent* ProjectileSpawnPoint;

	UPROPERTY(EditDefaultsOnly, Category = "Combat")
	TSubclassOf<class AProjectile> ProjectileClass;
};
