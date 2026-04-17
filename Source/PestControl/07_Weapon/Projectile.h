// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "20_System/Pool/PoolableInterface.h"
#include "Projectile.generated.h"

UCLASS()
class PESTCONTROL_API AProjectile : public AActor, public IPoolable
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

	/** 풀링 호환을 위해 멤버 컴포넌트로 유지. 최초 1회 Niagara System 지정 후 Activate/Deactivate로 재사용. */
	UPROPERTY(VisibleAnywhere, Category = "Effects")
	class UNiagaraComponent* TraceEffectComponent;

private:
	FVector StartLocation;

	/** 생성자/최초 BeginPlay에서 단 한 번만 콜리전 델리게이트 바인딩을 수행하기 위한 플래그. */
	bool bDelegatesBound = false;

	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	UFUNCTION()
	void OnBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	/** 델리게이트 바인딩 1회 수행. */
	void BindCollisionDelegates();

protected:
	/** Pool로 자기 자신을 반환 (풀이 없으면 Destroy로 폴백). 파생 클래스에서도 사용. */
	void ReleaseSelf();

public:

	UPROPERTY(EditAnywhere)
	float Damage = 50.f;

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void SetDamage(float InDamage);

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Projectile")
	float MaxDistance = 1000.f;

	UProjectileMovementComponent* GetProjectileMovementComponent() const;

	// ~ IPoolable
	virtual void OnAcquired_Implementation(const FTransform& SpawnTransform, AActor* NewOwner, APawn* NewInstigator) override;
	virtual void OnReleased_Implementation() override;
	// ~

};

