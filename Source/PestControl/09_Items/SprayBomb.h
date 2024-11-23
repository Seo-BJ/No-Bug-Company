// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SprayBomb.generated.h"

class UStaticMeshComponent;
class USphereComponent;

UCLASS()
class PESTCONTROL_API ASprayBomb : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASprayBomb();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

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



};
