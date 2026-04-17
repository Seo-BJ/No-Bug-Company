// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Tickable.h"
#include "PeCoBenchmarkSubsystem.generated.h"

/**
 * 오브젝트 풀 도입 전/후 비교용 벤치마크 수집기.
 *
 * 콘솔 커맨드:
 *   pe.Bench.Start <ScenarioName>   수집 시작
 *   pe.Bench.Stop                   수집 종료 + CSV 저장
 *
 * 수집 지표:
 *   - 프레임타임(ms) 전 프레임 저장 → 평균/최소/최대/1% Low/16.6ms 초과 Hitch
 *   - GC 이벤트(Pre/Post 델리게이트) → 횟수/1회 시간/합계
 *   - AProjectile / APeCoEnemyCharacter UObject 피크 수
 *   - WorkingSet 피크(MB)
 *   - Enemy Spawn (외부에서 RecordEnemySpawn 호출 시 µs 누적)
 *
 * 출력: Saved/Benchmarks/<timestamp>_<scenario>.csv
 *       1행: 요약 통계
 *       2행 이후: 프레임별 원본 (frame_idx, dt_ms)
 */
UCLASS()
class PESTCONTROL_API UPeCoBenchmarkSubsystem : public UGameInstanceSubsystem, public FTickableGameObject
{
	GENERATED_BODY()

public:
	//~ USubsystem
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;
	//~ End USubsystem

	//~ FTickableGameObject
	virtual void Tick(float DeltaTime) override;
	virtual bool IsTickable() const override { return bIsRunning || bAutoRunArmed; }
	virtual TStatId GetStatId() const override { RETURN_QUICK_DECLARE_CYCLE_STAT(UPeCoBenchmarkSubsystem, STATGROUP_Tickables); }
	virtual bool IsTickableWhenPaused() const override { return false; }
	virtual bool IsTickableInEditor() const override { return false; }
	//~ End FTickableGameObject

	static UPeCoBenchmarkSubsystem* Get(const UObject* WorldContext);

	void StartBenchmark(const FString& ScenarioName);
	void StopBenchmark();

	// 게임 시작 후 자동으로 Start/Stop 수행.
	// PIE/패키징 모두 월드가 처음 준비되면 StartDelaySeconds 뒤 Start,
	// 이어서 DurationSeconds 뒤 Stop 하고 CSV를 쓴다.
	static constexpr bool bAutoRunOnBoot = true;
	static constexpr float AutoRunStartDelaySeconds = 2.0f;
	static constexpr float AutoRunDurationSeconds = 60.0f;

	// 외부 훅. SpawnActor 전후 타이밍을 µs로 넘기면 누적.
	void RecordEnemySpawn(double ElapsedMicroseconds);

private:
	void HandlePreGC();
	void HandlePostGC();

	void SampleObjectCounts();
	void WriteCsv();

	// 자동 실행
	FString AutoRunScenarioName;
	bool bAutoRunArmed = false;
	float AutoRunStartCountdown = 0.f;
	float AutoRunStopCountdown = 0.f;
	void TickAutoRun(float DeltaTime);
	void OnWorldPostInit(UWorld* World, const UWorld::InitializationValues);
	FDelegateHandle WorldInitHandle;

	bool bIsRunning = false;

	FString CurrentScenario;
	double StartSeconds = 0.0;

	// 프레임타임 (ms)
	TArray<float> FrameTimesMs;

	// GC
	double GCStartSeconds = 0.0;
	int32 GCCount = 0;
	double GCTotalMs = 0.0;
	double GCMaxMs = 0.0;

	// Hitch
	int32 HitchCount = 0;
	static constexpr float HitchThresholdMs = 16.6f;

	// Enemy spawn 누적
	int32 EnemySpawnCount = 0;
	double EnemySpawnTotalUs = 0.0;
	double EnemySpawnMaxUs = 0.0;

	// Peak 샘플링
	int32 PeakProjectileCount = 0;
	int32 PeakEnemyCount = 0;
	uint64 PeakWorkingSetBytes = 0;
	float ObjectSampleAccumulator = 0.f;
	static constexpr float ObjectSampleIntervalSeconds = 0.25f;

	// GC 델리게이트 핸들
	FDelegateHandle PreGCHandle;
	FDelegateHandle PostGCHandle;

	// 콘솔 커맨드 핸들
	IConsoleCommand* CmdStart = nullptr;
	IConsoleCommand* CmdStop = nullptr;
};
