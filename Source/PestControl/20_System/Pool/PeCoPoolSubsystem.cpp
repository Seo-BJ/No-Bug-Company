// Fill out your copyright notice in the Description page of Project Settings.

#include "20_System/Pool/PeCoPoolSubsystem.h"
#include "20_System/Pool/PoolableInterface.h"

#include "Engine/World.h"
#include "Engine/Engine.h"
#include "GameFramework/Pawn.h"

const FVector UPeCoPoolSubsystem::ParkingLocation = FVector(0.f, 0.f, -100000.f);

void UPeCoPoolSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
}

// ---------- FTickableGameObject ----------
// Debug 빌드에서만 실제 작업(화면 디버그 출력). 다른 구성에서는 IsTickable가 false를 반환해 Tick이 호출되지 않는다.

bool UPeCoPoolSubsystem::IsTickable() const
{
#if PECO_POOL_DEBUG_HUD
	// PIE/게임 월드에서만 유효.
	const UWorld* World = GetWorld();
	return World && (World->WorldType == EWorldType::Game || World->WorldType == EWorldType::PIE);
#else
	return false;
#endif
}

TStatId UPeCoPoolSubsystem::GetStatId() const
{
	RETURN_QUICK_DECLARE_CYCLE_STAT(UPeCoPoolSubsystem, STATGROUP_Tickables);
}

void UPeCoPoolSubsystem::Tick(float DeltaTime)
{
#if PECO_POOL_DEBUG_HUD
	if (!GEngine)
	{
		return;
	}

	// 상단 헤더.
	GEngine->AddOnScreenDebugMessage(
		/*Key*/ 0xEC00,
		/*TimeToDisplay*/ 0.f,
		FColor::Cyan,
		TEXT("=== PeCo Object Pool ==="));

	// 고정 Key 범위(0xEC01 ~ 0xECFF)로 클래스별 1줄씩 갱신. Key 고정 덕에 프레임마다 덮어쓰기된다.
	int32 KeyOffset = 1;
	for (const auto& Pair : Pools)
	{
		const UClass* Cls = Pair.Key;
		const FActorPool& Pool = Pair.Value;
		if (!Cls)
		{
			continue;
		}

		const int32 Active = Pool.ActiveActors.Num();
		const int32 Inactive = Pool.InactiveActors.Num();
		const int32 Cold = Pool.TotalColdSpawnCount;
		const int32 Reuse = Pool.TotalReuseCount;
		const int32 Total = Cold + Reuse;
		const float ReusePct = (Total > 0) ? (100.f * static_cast<float>(Reuse) / static_cast<float>(Total)) : 0.f;

		const FString Line = FString::Printf(
			TEXT("%-36s | Active:%3d  Inactive:%3d  Cold:%4d  Reuse:%5d  (%.1f%%)"),
			*Cls->GetName(), Active, Inactive, Cold, Reuse, ReusePct);

		GEngine->AddOnScreenDebugMessage(
			/*Key*/ 0xEC00 + KeyOffset,
			/*TimeToDisplay*/ 0.f,
			FColor::Green,
			Line);

		++KeyOffset;
		if (KeyOffset > 0xFF) break; // Key 충돌 방지.
	}
#endif
}

void UPeCoPoolSubsystem::Deinitialize()
{
	// 모든 풀 액터를 확실히 정리. 월드 종료 시점에 댕글링 방지.
	for (auto& Pair : Pools)
	{
		FActorPool& Pool = Pair.Value;
		for (AActor* Actor : Pool.InactiveActors)
		{
			if (IsValid(Actor))
			{
				Actor->Destroy();
			}
		}
		Pool.InactiveActors.Reset();

		for (AActor* Actor : Pool.ActiveActors)
		{
			if (IsValid(Actor))
			{
				Actor->Destroy();
			}
		}
		Pool.ActiveActors.Reset();
	}
	Pools.Reset();

	Super::Deinitialize();
}

AActor* UPeCoPoolSubsystem::AcquireActor(TSubclassOf<AActor> Class, const FTransform& SpawnT, AActor* NewOwner, APawn* NewInstigator)
{
	if (!Class)
	{
		return nullptr;
	}

	UClass* Key = *Class;
	FActorPool& Pool = Pools.FindOrAdd(Key);

	AActor* Actor = nullptr;

	// 풀 히트: 비활성 스택에서 뽑는다. 유효하지 않은 엔트리(가비지 수거됨)는 버린다.
	while (Pool.InactiveActors.Num() > 0)
	{
		AActor* Candidate = Pool.InactiveActors.Pop(EAllowShrinking::No);
		if (IsValid(Candidate))
		{
			Actor = Candidate;
			Pool.TotalReuseCount++;
			break;
		}
	}

	// 풀 미스: 새로 스폰.
	if (!Actor)
	{
		Actor = InternalSpawnNew(Key, SpawnT, NewOwner, NewInstigator);
		if (!Actor)
		{
			return nullptr;
		}
		Pool.TotalColdSpawnCount++;
	}

	Pool.ActiveActors.Add(Actor);
	ActivateActor(Actor, SpawnT, NewOwner, NewInstigator);
	return Actor;
}

void UPeCoPoolSubsystem::ReleaseActor(AActor* Actor)
{
	if (!IsValid(Actor))
	{
		return;
	}

	UClass* Key = Actor->GetClass();
	FActorPool* Pool = Pools.Find(Key);
	if (!Pool)
	{
		// 풀로 관리되지 않는 액터가 실수로 들어온 경우 - 안전하게 Destroy.
		Actor->Destroy();
		return;
	}

	Pool->ActiveActors.Remove(Actor);

	// IPoolable::OnReleased 호출 (인터페이스 구현 시)
	if (Actor->GetClass()->ImplementsInterface(UPoolable::StaticClass()))
	{
		IPoolable::Execute_OnReleased(Actor);
	}

	DeactivateActor(Actor);

	// 풀 상한 체크.
	if (Pool->MaxSize > 0 && Pool->InactiveActors.Num() >= Pool->MaxSize)
	{
		Actor->Destroy();
		return;
	}

	Pool->InactiveActors.Add(Actor);
}

void UPeCoPoolSubsystem::PreWarm(TSubclassOf<AActor> Class, int32 Count)
{
	if (!Class || Count <= 0)
	{
		return;
	}

	UClass* Key = *Class;
	FActorPool& Pool = Pools.FindOrAdd(Key);

	for (int32 i = 0; i < Count; ++i)
	{
		AActor* NewActor = InternalSpawnNew(Key, FTransform(FRotator::ZeroRotator, ParkingLocation), nullptr, nullptr);
		if (!NewActor)
		{
			break;
		}
		Pool.TotalColdSpawnCount++;
		DeactivateActor(NewActor);
		Pool.InactiveActors.Add(NewActor);
	}
}

void UPeCoPoolSubsystem::SetMaxSize(TSubclassOf<AActor> Class, int32 InMaxSize)
{
	if (!Class)
	{
		return;
	}
	FActorPool& Pool = Pools.FindOrAdd(*Class);
	Pool.MaxSize = FMath::Max(0, InMaxSize);
}

int32 UPeCoPoolSubsystem::GetActiveCount(TSubclassOf<AActor> Class) const
{
	if (!Class) return 0;
	const FActorPool* Pool = Pools.Find(*Class);
	return Pool ? Pool->ActiveActors.Num() : 0;
}

int32 UPeCoPoolSubsystem::GetInactiveCount(TSubclassOf<AActor> Class) const
{
	if (!Class) return 0;
	const FActorPool* Pool = Pools.Find(*Class);
	return Pool ? Pool->InactiveActors.Num() : 0;
}

int32 UPeCoPoolSubsystem::GetColdSpawnCount(TSubclassOf<AActor> Class) const
{
	if (!Class) return 0;
	const FActorPool* Pool = Pools.Find(*Class);
	return Pool ? Pool->TotalColdSpawnCount : 0;
}

int32 UPeCoPoolSubsystem::GetReuseCount(TSubclassOf<AActor> Class) const
{
	if (!Class) return 0;
	const FActorPool* Pool = Pools.Find(*Class);
	return Pool ? Pool->TotalReuseCount : 0;
}

AActor* UPeCoPoolSubsystem::InternalSpawnNew(UClass* Class, const FTransform& SpawnT, AActor* NewOwner, APawn* NewInstigator)
{
	UWorld* World = GetWorld();
	if (!World || !Class)
	{
		return nullptr;
	}

	FActorSpawnParameters Params;
	Params.Owner = NewOwner;
	Params.Instigator = NewInstigator;
	// 대량 스폰 시 충돌 영역이 미세 겹치더라도 스폰 실패 없이 진행.
	Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	// 위치/회전만 전달하고 스케일은 CDO의 기본값을 유지한다.
	// (SpawnT.GetScale3D()를 그대로 사용하면 호출자가 FTransform(Rot, Loc)로 넘겼을 때
	//  Scale이 (1,1,1)로 덮어써져 Flying Enemy 등의 Capsule 스케일이 사라진다.)
	const FTransform SpawnLocRot(SpawnT.GetRotation(), SpawnT.GetLocation());
	return World->SpawnActor<AActor>(Class, SpawnLocRot, Params);
}

void UPeCoPoolSubsystem::ActivateActor(AActor* Actor, const FTransform& SpawnT, AActor* NewOwner, APawn* NewInstigator)
{
	if (!IsValid(Actor))
	{
		return;
	}

	// 1) Owner/Instigator 먼저 설정 (이후 OnAcquired에서 OwnerWeapon을 참조).
	Actor->SetOwner(NewOwner);
	Actor->SetInstigator(NewInstigator);

	// 2) 가시성/충돌/Tick 복구. ProjectileMovement가 Tick을 재개하기 "전에" 위치를 확정해야 한다.
	Actor->SetActorHiddenInGame(false);
	Actor->SetActorEnableCollision(true);
	Actor->SetActorTickEnabled(true);

	// 3) 위치/회전 배치.
	//    먼저 루트 컴포넌트에 직접 WorldLocation/Rotation을 강제로 지정해
	//    ParkingLocation 잔존 물리 상태를 확실히 밀어낸다. (SetActorTransform 단독으로는
	//    일부 설정의 ProjectileMovement/Physics에서 위치가 무시되는 사례가 있음)
	// 스케일은 의도적으로 건드리지 않는다 — 호출자가 기본 FTransform(Rot, Loc)를 넘기면
	// Scale=(1,1,1)로 덮어써져서 CDO에 설정된 스케일(예: Flying Enemy의 Capsule 스케일)이 사라진다.
	if (USceneComponent* Root = Actor->GetRootComponent())
	{
		Root->SetWorldLocationAndRotation(
			SpawnT.GetLocation(),
			SpawnT.GetRotation(),
			/*bSweep*/ false,
			/*OutHit*/ nullptr,
			ETeleportType::TeleportPhysics);
	}
	Actor->SetActorLocationAndRotation(
		SpawnT.GetLocation(),
		SpawnT.GetRotation(),
		/*bSweep*/ false,
		/*OutHit*/ nullptr,
		ETeleportType::TeleportPhysics);

	// 4) IPoolable::OnAcquired 호출 - 구현체에서 ProjectileMovement 재시동 등 수행.
	if (Actor->GetClass()->ImplementsInterface(UPoolable::StaticClass()))
	{
		IPoolable::Execute_OnAcquired(Actor, SpawnT, NewOwner, NewInstigator);
	}
}

void UPeCoPoolSubsystem::DeactivateActor(AActor* Actor)
{
	if (!IsValid(Actor))
	{
		return;
	}

	Actor->SetActorHiddenInGame(true);
	Actor->SetActorEnableCollision(false);
	Actor->SetActorTickEnabled(false);

	// 풀 대기소로 이동 (월드에서 보이지 않는 위치). 콜리전은 이미 꺼져 있으므로 텔레포트 OK.
	Actor->SetActorLocation(ParkingLocation, /*bSweep*/ false, /*OutHit*/ nullptr, ETeleportType::TeleportPhysics);
}
