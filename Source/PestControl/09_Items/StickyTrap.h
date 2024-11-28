// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "09_Items/CombatItem.h"
#include "StickyTrap.generated.h"

class UBoxComponent;
class UStaticMeshComponent;
class APeCoEnemyCharacter;

UCLASS()
class PESTCONTROL_API AStickyTrap : public ACombatItem
{
	GENERATED_BODY()
	
public:
	AStickyTrap();

	void UseCombatItem() override;

protected:
	virtual void BeginPlay() override;

public:
	UFUNCTION()
	void OnEnemyOverlapped(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	void ActivateTrap();
	void DeactivateTrap();
	void DisableMove();
	void EnableMove();

private:
	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* TrapMesh;

	UPROPERTY(VisibleAnywhere)
	UBoxComponent* DetectionBox;

	UPROPERTY(EditAnywhere, Category = "Trap Settings")
	float TrapDuration = 5.0f;

	TArray<APeCoEnemyCharacter*> OverlappingEnemies;

	TMap<APeCoEnemyCharacter*, float> OriginalSpeeds;

	bool bIsTrapActive;

};
