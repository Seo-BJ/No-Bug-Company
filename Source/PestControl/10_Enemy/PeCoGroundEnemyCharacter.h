// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "01_Character/PeCoEnemyCharacter.h"
#include "PeCoGroundEnemyCharacter.generated.h"

/**
 * 
 */
UCLASS()
class PESTCONTROL_API APeCoGroundEnemyCharacter : public APeCoEnemyCharacter
{
	GENERATED_BODY()

public:
	
	APeCoGroundEnemyCharacter();

protected:
	
	virtual void BeginPlay() override;
	virtual void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit) override;

private:
		
	// Temporarily stop AI movement during knockback
	void StopAIMovementDuringKnockback(float Duration);
	
	// Timer handle to manage knockback recovery time
	FTimerHandle KnockbackTimerHandle;
};