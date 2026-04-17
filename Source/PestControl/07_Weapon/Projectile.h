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
	class UCapsuleComponent* RootCollisionComponent;

	UPROPERTY(EditDefaultsOnly, Category = "Combat")
	UStaticMeshComponent* ProjectileMesh;

	UPROPERTY(VisibleAnywhere, Category = "Movement")
	class UProjectileMovementComponent* ProjectileMovementComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effects")
	class UNiagaraSystem* NiagaraImpactEffect;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effects")
	FVector ImpactEffectScale = FVector(0.3f);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effects")
	class UNiagaraSystem* NiagaraTraceEffect;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effects")
	FVector TraceEffectScale = FVector(0.3f);

	UPROPERTY()
	class UNiagaraComponent* ActiveTraceEffect;

private:
	FVector StartLocation;

	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	UFUNCTION()
	void OnBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

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
