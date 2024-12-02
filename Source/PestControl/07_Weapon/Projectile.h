// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Projectile.generated.h"

UCLASS()
class PESTCONTROL_API AProjectile : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AProjectile();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;


	UPROPERTY(VisibleAnywhere, Category = "Components")
	USceneComponent* RootSceneComponent;

	UPROPERTY(EditDefaultsOnly, Category = "Combat")
	UStaticMeshComponent* ProjectileMesh;

	UPROPERTY(VisibleAnywhere, Category = "Movement")
	class UProjectileMovementComponent* ProjectileMovementComponent;


private:
	FVector StartLocation;

	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	//void SpawnFragmentProjectiles(const FVector& SpawnLocation, const FRotator& SpawnRotation);

public:	

	UPROPERTY(EditAnywhere)
	float Damage = 50.f;

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void SetDamage(float InDamage);

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Projectile")
	float MaxDistance = 1000.f;

	UProjectileMovementComponent* GetProjectileMovementComponent() const;
};
