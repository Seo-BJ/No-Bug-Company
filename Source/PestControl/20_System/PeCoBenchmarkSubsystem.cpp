// Fill out your copyright notice in the Description page of Project Settings.

#include "20_System/PeCoBenchmarkSubsystem.h"

#include "07_Weapon/Projectile.h"
#include "01_Character/PeCoEnemyCharacter.h"

#include "Engine/GameInstance.h"
#include "Engine/World.h"
#include "HAL/PlatformTime.h"
#include "HAL/PlatformMemory.h"
#include "HAL/IConsoleManager.h"
#include "HAL/FileManager.h"
#include "Misc/Paths.h"
#include "Misc/FileHelper.h"
#include "Misc/DateTime.h"
#include "UObject/UObjectIterator.h"
#include "UObject/UObjectGlobals.h"

void UPeCoBenchmarkSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	PreGCHandle = FCoreUObjectDelegates::GetPreGarbageCollectDelegate().AddUObject(this, &UPeCoBenchmarkSubsystem::HandlePreGC);
	PostGCHandle = FCoreUObjectDelegates::GetPostGarbageCollect().AddUObject(this, &UPeCoBenchmarkSubsystem::HandlePostGC);

	CmdStart = IConsoleManager::Get().RegisterConsoleCommand(
		TEXT("pe.Bench.Start"),
		TEXT("Start benchmark capture. Usage: pe.Bench.Start <ScenarioName>"),
		FConsoleCommandWithArgsDelegate::CreateLambda([this](const TArray<FString>& Args)
		{
			const FString Name = Args.Num() > 0 ? Args[0] : TEXT("Scenario");
			StartBenchmark(Name);
		}),
		ECVF_Default);

	CmdStop = IConsoleManager::Get().RegisterConsoleCommand(
		TEXT("pe.Bench.Stop"),
		TEXT("Stop benchmark capture and write CSV."),
		FConsoleCommandDelegate::CreateLambda([this]()
		{
			StopBenchmark();
		}),
		ECVF_Default);

	if (bAutoRunOnBoot)
	{
		// 게임 월드가 만들어지는 순간 자동 실행을 예약한다.
		WorldInitHandle = FWorldDelegates::OnPostWorldInitialization.AddUObject(
			this, &UPeCoBenchmarkSubsystem::OnWorldPostInit);
	}
}

void UPeCoBenchmarkSubsystem::Deinitialize()
{
	if (bIsRunning)
	{
		StopBenchmark();
	}

	FCoreUObjectDelegates::GetPreGarbageCollectDelegate().Remove(PreGCHandle);
	FCoreUObjectDelegates::GetPostGarbageCollect().Remove(PostGCHandle);

	if (WorldInitHandle.IsValid())
	{
		FWorldDelegates::OnPostWorldInitialization.Remove(WorldInitHandle);
		WorldInitHandle.Reset();
	}

	if (CmdStart)
	{
		IConsoleManager::Get().UnregisterConsoleObject(CmdStart);
		CmdStart = nullptr;
	}
	if (CmdStop)
	{
		IConsoleManager::Get().UnregisterConsoleObject(CmdStop);
		CmdStop = nullptr;
	}

	Super::Deinitialize();
}

UPeCoBenchmarkSubsystem* UPeCoBenchmarkSubsystem::Get(const UObject* WorldContext)
{
	if (!WorldContext) return nullptr;
	if (const UWorld* World = WorldContext->GetWorld())
	{
		if (UGameInstance* GI = World->GetGameInstance())
		{
			return GI->GetSubsystem<UPeCoBenchmarkSubsystem>();
		}
	}
	return nullptr;
}

void UPeCoBenchmarkSubsystem::StartBenchmark(const FString& ScenarioName)
{
	if (bIsRunning)
	{
		UE_LOG(LogTemp, Warning, TEXT("[Bench] Already running (%s). Stop first."), *CurrentScenario);
		return;
	}

	CurrentScenario = ScenarioName;
	FrameTimesMs.Reset();
	FrameTimesMs.Reserve(60 * 300); // 60fps * 5min 예약

	GCCount = 0;
	GCTotalMs = 0.0;
	GCMaxMs = 0.0;
	HitchCount = 0;

	EnemySpawnCount = 0;
	EnemySpawnTotalUs = 0.0;
	EnemySpawnMaxUs = 0.0;

	PeakProjectileCount = 0;
	PeakEnemyCount = 0;
	PeakWorkingSetBytes = 0;
	ObjectSampleAccumulator = 0.f;

	StartSeconds = FPlatformTime::Seconds();
	bIsRunning = true;

	UE_LOG(LogTemp, Warning, TEXT("[Bench] Start scenario=%s"), *CurrentScenario);
}

void UPeCoBenchmarkSubsystem::StopBenchmark()
{
	if (!bIsRunning)
	{
		UE_LOG(LogTemp, Warning, TEXT("[Bench] Not running."));
		return;
	}
	bIsRunning = false;
	WriteCsv();
}

void UPeCoBenchmarkSubsystem::Tick(float DeltaTime)
{
	if (bAutoRunArmed)
	{
		TickAutoRun(DeltaTime);
	}

	if (!bIsRunning) return;

	const float DtMs = DeltaTime * 1000.f;
	FrameTimesMs.Add(DtMs);
	if (DtMs > HitchThresholdMs)
	{
		++HitchCount;
	}

	ObjectSampleAccumulator += DeltaTime;
	if (ObjectSampleAccumulator >= ObjectSampleIntervalSeconds)
	{
		ObjectSampleAccumulator = 0.f;
		SampleObjectCounts();
	}
}

void UPeCoBenchmarkSubsystem::RecordEnemySpawn(double ElapsedMicroseconds)
{
	if (!bIsRunning) return;
	++EnemySpawnCount;
	EnemySpawnTotalUs += ElapsedMicroseconds;
	if (ElapsedMicroseconds > EnemySpawnMaxUs)
	{
		EnemySpawnMaxUs = ElapsedMicroseconds;
	}
}

void UPeCoBenchmarkSubsystem::HandlePreGC()
{
	if (!bIsRunning) return;
	GCStartSeconds = FPlatformTime::Seconds();
}

void UPeCoBenchmarkSubsystem::HandlePostGC()
{
	if (!bIsRunning) return;
	const double ElapsedMs = (FPlatformTime::Seconds() - GCStartSeconds) * 1000.0;
	++GCCount;
	GCTotalMs += ElapsedMs;
	if (ElapsedMs > GCMaxMs)
	{
		GCMaxMs = ElapsedMs;
	}
}

void UPeCoBenchmarkSubsystem::SampleObjectCounts()
{
	int32 ProjCount = 0;
	int32 EnemyCount = 0;

	for (TObjectIterator<AProjectile> It; It; ++It)
	{
		if (IsValid(*It)) ++ProjCount;
	}
	for (TObjectIterator<APeCoEnemyCharacter> It; It; ++It)
	{
		if (IsValid(*It)) ++EnemyCount;
	}

	if (ProjCount > PeakProjectileCount) PeakProjectileCount = ProjCount;
	if (EnemyCount > PeakEnemyCount) PeakEnemyCount = EnemyCount;

	const FPlatformMemoryStats MemStats = FPlatformMemory::GetStats();
	if (MemStats.UsedPhysical > PeakWorkingSetBytes)
	{
		PeakWorkingSetBytes = MemStats.UsedPhysical;
	}
}

void UPeCoBenchmarkSubsystem::WriteCsv()
{
	const double TotalSeconds = FPlatformTime::Seconds() - StartSeconds;
	const int32 FrameCount = FrameTimesMs.Num();

	// 통계 계산
	float AvgMs = 0.f, MinMs = 0.f, MaxMs = 0.f, OnePercentLowFps = 0.f;
	if (FrameCount > 0)
	{
		double Sum = 0.0;
		MinMs = FrameTimesMs[0];
		MaxMs = FrameTimesMs[0];
		for (float Dt : FrameTimesMs)
		{
			Sum += Dt;
			if (Dt < MinMs) MinMs = Dt;
			if (Dt > MaxMs) MaxMs = Dt;
		}
		AvgMs = static_cast<float>(Sum / FrameCount);

		// 1% Low FPS: 상위 1% 느린 프레임들의 평균 프레임타임 → FPS
		TArray<float> Sorted = FrameTimesMs;
		Sorted.Sort([](const float& A, const float& B){ return A > B; }); // 내림차순 (느린 프레임 먼저)
		const int32 OnePctN = FMath::Max(1, FrameCount / 100);
		double SlowSum = 0.0;
		for (int32 i = 0; i < OnePctN; ++i) SlowSum += Sorted[i];
		const double SlowAvgMs = SlowSum / OnePctN;
		OnePercentLowFps = SlowAvgMs > 0.0 ? static_cast<float>(1000.0 / SlowAvgMs) : 0.f;
	}

	const float AvgFps = AvgMs > 0.f ? 1000.f / AvgMs : 0.f;
	const float AvgSpawnUs = EnemySpawnCount > 0 ? static_cast<float>(EnemySpawnTotalUs / EnemySpawnCount) : 0.f;
	const float AvgGCMs = GCCount > 0 ? static_cast<float>(GCTotalMs / GCCount) : 0.f;
	const float PeakMemMB = static_cast<float>(PeakWorkingSetBytes) / (1024.f * 1024.f);

	// CSV 빌드
	FString Csv;
	Csv += TEXT("scenario,duration_s,frames,avg_fps,avg_ms,min_ms,max_ms,one_percent_low_fps,hitch_count,")
		   TEXT("gc_count,gc_total_ms,gc_avg_ms,gc_max_ms,")
		   TEXT("enemy_spawn_count,enemy_spawn_avg_us,enemy_spawn_max_us,")
		   TEXT("peak_projectile_count,peak_enemy_count,peak_working_set_mb\n");

	Csv += FString::Printf(
		TEXT("%s,%.3f,%d,%.2f,%.3f,%.3f,%.3f,%.2f,%d,%d,%.3f,%.3f,%.3f,%d,%.2f,%.2f,%d,%d,%.2f\n"),
		*CurrentScenario, TotalSeconds, FrameCount,
		AvgFps, AvgMs, MinMs, MaxMs, OnePercentLowFps, HitchCount,
		GCCount, GCTotalMs, AvgGCMs, GCMaxMs,
		EnemySpawnCount, AvgSpawnUs, EnemySpawnMaxUs,
		PeakProjectileCount, PeakEnemyCount, PeakMemMB);

	Csv += TEXT("\nframe_idx,dt_ms\n");
	for (int32 i = 0; i < FrameCount; ++i)
	{
		Csv += FString::Printf(TEXT("%d,%.4f\n"), i, FrameTimesMs[i]);
	}

	// 경로: <Project>/Saved/Benchmarks/<timestamp>_<scenario>.csv
	const FString Stamp = FDateTime::Now().ToString(TEXT("%Y%m%d_%H%M%S"));
	const FString SafeName = CurrentScenario.Replace(TEXT(" "), TEXT("_"));
	const FString FileName = FString::Printf(TEXT("%s_%s.csv"), *Stamp, *SafeName);
	const FString Dir = FPaths::Combine(FPaths::ProjectSavedDir(), TEXT("Benchmarks"));
	const FString FullPath = FPaths::Combine(Dir, FileName);

	IFileManager::Get().MakeDirectory(*Dir, true);
	const bool bSaved = FFileHelper::SaveStringToFile(Csv, *FullPath);

	UE_LOG(LogTemp, Warning,
		TEXT("[Bench] Stop scenario=%s | frames=%d avgFPS=%.1f 1%%Low=%.1f hitch=%d | GC=%d(%.1fms total) | spawns=%d(avg %.1fus) | peak Proj=%d Enemy=%d Mem=%.1fMB | saved=%d path=%s"),
		*CurrentScenario, FrameCount, AvgFps, OnePercentLowFps, HitchCount,
		GCCount, GCTotalMs, EnemySpawnCount, AvgSpawnUs,
		PeakProjectileCount, PeakEnemyCount, PeakMemMB,
		bSaved ? 1 : 0, *FullPath);
}

void UPeCoBenchmarkSubsystem::OnWorldPostInit(UWorld* World, const UWorld::InitializationValues)
{
	// 게임 월드에서만 자동 실행 (에디터 미리보기 월드 무시).
	if (!World) return;
	if (World->WorldType != EWorldType::Game && World->WorldType != EWorldType::PIE)
	{
		return;
	}
	if (bAutoRunArmed || bIsRunning) return;

	AutoRunScenarioName = FString::Printf(TEXT("Auto_%s"), *World->GetMapName());
	AutoRunStartCountdown = AutoRunStartDelaySeconds;
	AutoRunStopCountdown = AutoRunDurationSeconds;
	bAutoRunArmed = true;

	UE_LOG(LogTemp, Warning,
		TEXT("[Bench] Auto-run armed. scenario=%s startDelay=%.1fs duration=%.1fs"),
		*AutoRunScenarioName, AutoRunStartDelaySeconds, AutoRunDurationSeconds);
}

void UPeCoBenchmarkSubsystem::TickAutoRun(float DeltaTime)
{
	if (!bIsRunning)
	{
		// 시작 카운트다운
		AutoRunStartCountdown -= DeltaTime;
		if (AutoRunStartCountdown <= 0.f)
		{
			StartBenchmark(AutoRunScenarioName);
		}
		return;
	}

	// 측정 중이면 종료 카운트다운
	AutoRunStopCountdown -= DeltaTime;
	if (AutoRunStopCountdown <= 0.f)
	{
		StopBenchmark();
		bAutoRunArmed = false;
	}
}
