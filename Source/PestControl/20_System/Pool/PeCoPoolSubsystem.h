// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "Tickable.h"
#include "GameFramework/Actor.h"
#include "PeCoPoolSubsystem.generated.h"

// 디버그 HUD 출력 활성화 조건.
// - Shipping / Test 빌드에서는 비활성화 (출시/측정 빌드 성능 보호).
// - 그 외 Debug / DebugGame / Development(+Editor)에서는 활성화.
// 포트폴리오 FPS 측정은 Shipping으로 돌려야 HUD가 간섭하지 않음.
#ifndef PECO_POOL_DEBUG_HUD
    #if UE_BUILD_SHIPPING || UE_BUILD_TEST
        #define PECO_POOL_DEBUG_HUD 0
    #else
        #define PECO_POOL_DEBUG_HUD 1
    #endif
#endif

/**
 * 클래스별 풀 데이터. TArray 기반의 간단한 스택.
 */
USTRUCT()
struct FActorPool
{
	GENERATED_BODY()

	/** 비활성 상태로 대기 중인 액터 (재사용 후보) */
	UPROPERTY()
	TArray<TObjectPtr<AActor>> InactiveActors;

	/** 현재 풀에서 꺼내져 활성 상태인 액터 */
	UPROPERTY()
	TSet<TObjectPtr<AActor>> ActiveActors;

	/** 풀 최대 보관 수. 초과 시 Release에서 실제 Destroy로 보냄. 0 = 무제한 */
	int32 MaxSize = 0;

	/** 누적 Cold Spawn(풀 미스로 SpawnActor 실제 호출한 수) - 포트폴리오 측정용 */
	int32 TotalColdSpawnCount = 0;

	/** 누적 Reuse(풀 히트로 재사용한 수) - 포트폴리오 측정용 */
	int32 TotalReuseCount = 0;
};

/**
 * 월드 범위 오브젝트 풀 서브시스템.
 *
 * 사용:
 *   auto* Pool = GetWorld()->GetSubsystem<UPeCoPoolSubsystem>();
 *   auto* P = Pool->Acquire<AProjectile>(BulletClass, FTransform(Rot, Loc), this, GetInstigator());
 *   ...
 *   Pool->ReleaseActor(P);
 *
 * 대상 액터가 IPoolable을 구현하면 OnAcquired/OnReleased가 자동 호출된다.
 */
UCLASS()
class PESTCONTROL_API UPeCoPoolSubsystem : public UWorldSubsystem, public FTickableGameObject
{
	GENERATED_BODY()

public:
	// USubsystem
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	// FTickableGameObject - 디버그 HUD 출력 전용. 빌드 구성에 따라 NoOp.
	virtual void Tick(float DeltaTime) override;
	virtual TStatId GetStatId() const override;
	virtual bool IsTickable() const override;
	virtual bool IsTickableInEditor() const override { return false; }
	virtual bool IsTickableWhenPaused() const override { return false; }
	virtual UWorld* GetTickableGameObjectWorld() const override { return GetWorld(); }

	/**
	 * 풀에서 액터를 획득. 없으면 새로 스폰.
	 * @param Class        획득할 액터 클래스 (TSubclassOf 허용).
	 * @param SpawnT       액터를 배치할 위치/회전.
	 * @param NewOwner     SetOwner에 사용. 기존 SpawnActor의 Owner와 동일.
	 * @param NewInstigator SetInstigator에 사용.
	 */
	AActor* AcquireActor(TSubclassOf<AActor> Class, const FTransform& SpawnT, AActor* NewOwner, APawn* NewInstigator);

	/** 템플릿 편의 함수. */
	template<typename T>
	T* Acquire(TSubclassOf<T> Class, const FTransform& SpawnT, AActor* NewOwner, APawn* NewInstigator)
	{
		return Cast<T>(AcquireActor(*Class, SpawnT, NewOwner, NewInstigator));
	}

	/** 사용이 끝난 액터를 풀로 반환. 풀 상한 초과 시 실제 Destroy. */
	UFUNCTION(BlueprintCallable, Category = "Pool")
	void ReleaseActor(AActor* Actor);

	/** 워밍업. 한 프레임에 Count 만큼 미리 생성하여 비활성 상태로 풀에 넣어둔다. */
	UFUNCTION(BlueprintCallable, Category = "Pool")
	void PreWarm(TSubclassOf<AActor> Class, int32 Count);

	/** 풀 상한을 설정. 0 = 무제한. */
	UFUNCTION(BlueprintCallable, Category = "Pool")
	void SetMaxSize(TSubclassOf<AActor> Class, int32 InMaxSize);

	// ------------------ 디버그 / 측정용 ------------------

	UFUNCTION(BlueprintCallable, Category = "Pool|Debug")
	int32 GetActiveCount(TSubclassOf<AActor> Class) const;

	UFUNCTION(BlueprintCallable, Category = "Pool|Debug")
	int32 GetInactiveCount(TSubclassOf<AActor> Class) const;

	UFUNCTION(BlueprintCallable, Category = "Pool|Debug")
	int32 GetColdSpawnCount(TSubclassOf<AActor> Class) const;

	UFUNCTION(BlueprintCallable, Category = "Pool|Debug")
	int32 GetReuseCount(TSubclassOf<AActor> Class) const;

private:
	/** 클래스별 풀 테이블. 키는 UClass*. */
	UPROPERTY()
	TMap<TObjectPtr<UClass>, FActorPool> Pools;

	/** 비활성 시 액터를 숨겨둘 위치 (월드에서 보이지 않는 곳). */
	static const FVector ParkingLocation;

	/** 실제 SpawnActor 호출. */
	AActor* InternalSpawnNew(UClass* Class, const FTransform& SpawnT, AActor* NewOwner, APawn* NewInstigator);

	/** 비활성화: Hidden / NoCollision / TickDisable / 위치 숨김. */
	void DeactivateActor(AActor* Actor);

	/** 활성화: Visible / Collision / Tick / 위치 반영. */
	void ActivateActor(AActor* Actor, const FTransform& SpawnT, AActor* NewOwner, APawn* NewInstigator);
};
