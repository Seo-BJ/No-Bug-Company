// Fill out your copyright notice in the Description page of Project Settings.

#include "20_System/Pool/PeCoPoolSubsystem.h"
#include "20_System/Pool/PoolableInterface.h"
#include "20_System/Pool/PeCoPoolDeveloperSettings.h"
#include "PestControl.h"

#include "01_Character/PeCoEnemyCharacter.h"
#include "07_Weapon/Projectile.h"

#include "Engine/World.h"
#include "Engine/Engine.h"
#include "GameFramework/Pawn.h"

const FVector UPeCoPoolSubsystem::ParkingLocation = FVector(0.f, 0.f, -100000.f);

void UPeCoPoolSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
}

// ---------- FTickableGameObject ----------
// 개인 에디터 설정에서 HUD를 켠 경우에만 실제 작업(화면 디버그 출력).

bool UPeCoPoolSubsystem::IsTickable() const
{
	const UPeCoPoolDeveloperSettings* Settings = GetDefault<UPeCoPoolDeveloperSettings>();
	if (!Settings || !Settings->bEnableDebugHUD)
	{
		return false;
	}

	// PIE/게임 월드에서만 유효.
	const UWorld* World = GetWorld();
	return World && (World->WorldType == EWorldType::Game || World->WorldType == EWorldType::PIE);
}

TStatId UPeCoPoolSubsystem::GetStatId() const
{
	RETURN_QUICK_DECLARE_CYCLE_STAT(UPeCoPoolSubsystem, STATGROUP_Tickables);
}

void UPeCoPoolSubsystem::Tick(float DeltaTime)
{
	if (!GEngine)
	{
		return;
	}

	const UPeCoPoolDeveloperSettings* Settings = GetDefault<UPeCoPoolDeveloperSettings>();
	if (!Settings || !Settings->bEnableDebugHUD)
	{
		return;
	}

	// 상단 헤더에 현재 풀링 설정도 함께 표시.
	const FString Header = FString::Printf(
		TEXT("=== PeCo Object Pool | Enemy:%s Projectile:%s ==="),
		Settings->bEnableEnemyPooling ? TEXT("ON") : TEXT("OFF"),
		Settings->bEnableProjectilePooling ? TEXT("ON") : TEXT("OFF"));
	GEngine->AddOnScreenDebugMessage(
		/*Key*/ 0xEC00,
		/*TimeToDisplay*/ 0.f,
		FColor::Cyan,
		Header);

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

	// 해당 Actor 계열의 풀링이 꺼져 있으면 기존 SpawnActor 경로로 동작한다.
	if (!IsPoolingEnabledForClass(*Class))
	{
		return InternalSpawnNew(*Class, SpawnT, NewOwner, NewInstigator);
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
	if (!IsPoolingEnabledForClass(Key))
	{
		// PIE 중 설정을 끄더라도 기존 Active 집합에 대기 포인터가 남지 않게 정리.
		if (FActorPool* ExistingPool = Pools.Find(Key))
		{
			ExistingPool->ActiveActors.Remove(Actor);
		}
		Actor->Destroy();
		return;
	}

	FActorPool* Pool = Pools.Find(Key);
	if (!Pool)
	{
		// 풀로 관리되지 않는 액터가 실수로 들어온 경우 - 안전하게 Destroy.
		Actor->Destroy();
		return;
	}

	// 멱등성 가드: 동일 액터가 두 번 Release되어 Inactive 리스트에 중복 삽입되면
	// 다음 Acquire에서 같은 인스턴스가 동시에 두 번 활성화될 수 있다.
	if (Pool->InactiveActors.Contains(Actor))
	{
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
	if (!IsPoolingEnabledForClass(Key))
	{
		return;
	}

	FActorPool& Pool = Pools.FindOrAdd(Key);

	// Count는 "이 풀이 최소한 이 정도의 Inactive를 갖도록 보장하라"는 목표치로 해석한다.
	// 과거에는 Count만큼 "추가로" 생성하는 의미라 Spawner가 18개 있으면 Spawner마다
	// PreWarm(10)을 호출할 때 풀에 180체가 쌓이는 버그가 있었다 (동시 Active는 10체 내외).
	// 이 설계를 "목표 Inactive 수"로 바꾸면 여러 Spawner가 각자 같은 Count로 호출해도
	// 풀이 한 번만 10체로 채워지고 나머지 호출은 No-op가 된다.
	const int32 Needed = Count - Pool.InactiveActors.Num();
	if (Needed <= 0)
	{
		return;
	}

	for (int32 i = 0; i < Needed; ++i)
	{
		AActor* NewActor = InternalSpawnNew(Key, FTransform(FRotator::ZeroRotator, ParkingLocation), nullptr, nullptr);
		if (!NewActor)
		{
			break;
		}
		Pool.TotalColdSpawnCount++;

		// 런타임 Release와 동일한 비활성 상태로 맞춘다. IPoolable 구현체가 있으면
		// OnReleased를 태워 CharMovement/Anim/AI Tick까지 차단. 이게 빠지면
		// PreWarm된 Actor가 풀 대기 중에도 Component Tick을 계속 돌려
		// "진짜 비활성"이 되지 않는다.
		if (NewActor->GetClass()->ImplementsInterface(UPoolable::StaticClass()))
		{
			IPoolable::Execute_OnReleased(NewActor);
		}
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

bool UPeCoPoolSubsystem::IsPoolingEnabledForClass(const UClass* Class) const
{
	if (!Class)
	{
		return false;
	}

	const UPeCoPoolDeveloperSettings* Settings = GetDefault<UPeCoPoolDeveloperSettings>();
	if (!Settings)
	{
		return false;
	}

	if (Class->IsChildOf(AProjectile::StaticClass()))
	{
		return Settings->bEnableProjectilePooling;
	}
	if (Class->IsChildOf(APeCoEnemyCharacter::StaticClass()))
	{
		return Settings->bEnableEnemyPooling;
	}

	// 설정이 지정하는 두 계열 외의 IPoolable 확장 타입은 기존 풀 동작을 유지한다.
	return true;
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

void UPeCoPoolSubsystem::ActivateActor(
	AActor* Actor, const FTransform& SpawnT, AActor* NewOwner, APawn* NewInstigator)
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

	// 3) 위치/회전 배치. 루트 컴포넌트에 TeleportPhysics로 한 번만 지정한다.
	//    (과거에는 Actor 레벨 SetActorLocationAndRotation도 같이 태웠지만,
	//     Character/Projectile 모두 루트에 TeleportPhysics가 가면 위치·물리가 일관되게
	//     반영되며, 물리 씬 갱신을 중복으로 유발하지 않아 Acquire 비용이 낮아진다.)
	// 스케일은 건드리지 않는다 — 호출자가 기본 FTransform(Rot, Loc)를 넘기면 Scale=(1,1,1)로
	// 덮어써져 CDO 설정 스케일(예: Flying Enemy Capsule)이 사라진다.
	if (USceneComponent* Root = Actor->GetRootComponent())
	{
		Root->SetWorldLocationAndRotation(
			SpawnT.GetLocation(),
			SpawnT.GetRotation(),
			/*bSweep*/ false,
			/*OutHit*/ nullptr,
			ETeleportType::TeleportPhysics);
	}
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
