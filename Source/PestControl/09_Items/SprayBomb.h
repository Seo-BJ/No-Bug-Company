// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "09_Items/CombatItem.h"
#include "SprayBomb.generated.h"

class UStaticMeshComponent;
class USphereComponent;

UCLASS()
class PESTCONTROL_API ASprayBomb : public ACombatItem
{
	GENERATED_BODY()
	
public:	
	
	ASprayBomb();
	void UseCombatItem() override;

protected:

	virtual void BeginPlay() override;

public:	

	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* BombMesh;

private:

	UPROPERTY(VisibleAnywhere)
	USphereComponent* ExplodeRange;

	UPROPERTY(EditAnywhere, Category = "Slow")
	float SlowMultiplier = 0.5f;

	UPROPERTY(EditAnywhere, Category = "Slow")
	float MaxHealthDamageMultiplier = 0.5f;

	void Explode();

	void ApplySlowEffect(class APeCoEnemyCharacter* EnemyCharacter);

	bool bApplySlowEffect = true;

	float SlowDuration = 3.0f;

	UPROPERTY(EditAnywhere, Category = "SprayBomb")
	float BombRange = 1000.0f;

	void ThrowGrenade();

	FVector GetCursorLocation();

};
