// Fill out your copyright notice in the Description page of Project Settings.


#include "01_Character/PeCoEnemyCharacter.h"

#include "00_GameModes/PeCoGameMode.h"
#include "01_Character/PeCoPlayerCharacter.h"
#include "02_Player/PeCoPlayerState.h"
#include "07_Weapon/ConicalWeapon/Flamethrower.h"
#include "07_Weapon/ProjectileWeapon/LarvaLauncher.h"
#include "20_System/PeCoGameInstance.h"
#include "20_System/Pool/PeCoPoolSubsystem.h"
#include "21_Data/PeCoDataRow.h"

#include "AIController.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"

#include "Engine/StreamableManager.h"
#include "Engine/AssetManager.h"
#include "GameFramework/DamageType.h"
#include "HAL/IConsoleManager.h"

static TAutoConsoleVariable<float> CVarEnemyAutoKillLifetime(
	TEXT("pe.EnemyAutoKillLifetime"),
	0.0f,
	TEXT("If > 0, every spawned enemy auto-kills itself after this many seconds. Benchmark only."),
	ECVF_Cheat);

APeCoEnemyCharacter::APeCoEnemyCharacter()
{
	PrimaryActorTick.bCanEverTick = true;
		
	// Set KnockbackForce
	KnockbackForce = 1000.0f;

	// Set up Hit event
	GetCapsuleComponent()->OnComponentHit.AddDynamic(this, &APeCoEnemyCharacter::OnHit);

	// �⺻ EnemyID �� ���� (�ʿ�� �ڽ� Ŭ�������� �����)
	EnemyID = NAME_None;


	MaxHealth = 100.0f; // set max health
	Health = MaxHealth; // when the game start, set health = max health

	// 기본값 초기화
	bIsImmune = true; // 스폰 시 기본적으로 무적 상태
}

void APeCoEnemyCharacter::BeginPlay()
{
	Super::BeginPlay();
	OnTakeAnyDamage.AddDynamic(this, &APeCoEnemyCharacter::ReceiveDamage);
	SetTeam(ETeam::ET_Enemy);


	// 무적 상태 활성화
	bIsImmune = true;

	// 무적 상태 유지 시간 이후 해제
	GetWorld()->GetTimerManager().SetTimer(
		SpawnImmunityTimerHandle,
		this,
		&APeCoEnemyCharacter::RemoveSpawnImmunity,
		SpawnImmunityTime,
		false
	);

	// 자동 사망: Spawner가 주입한 값이 있으면 그걸 우선, 아니면 전역 CVar 값 사용.
	const float CVarLifetime = CVarEnemyAutoKillLifetime.GetValueOnGameThread();
	const float EffectiveLifetime = AutoKillLifetime > 0.f ? AutoKillLifetime : CVarLifetime;
	if (EffectiveLifetime > 0.f)
	{
		ScheduleAutoKill(EffectiveLifetime);
	}
}

void APeCoEnemyCharacter::ScheduleAutoKill(float Lifetime)
{
	if (Lifetime <= 0.f || !GetWorld())
	{
		return;
	}
	AutoKillLifetime = Lifetime;
	GetWorld()->GetTimerManager().SetTimer(
		AutoKillTimerHandle,
		this,
		&APeCoEnemyCharacter::HandleAutoKill,
		Lifetime,
		false
	);
}

void APeCoEnemyCharacter::HandleAutoKill()
{
	if (!IsValid(this) || Health <= 0.f)
	{
		return;
	}
	// 무적이면 해제하고 진행 (벤치 시나리오에서 수명을 정확히 맞추기 위함)
	bIsImmune = false;

	UGameplayStatics::ApplyDamage(
		this,
		MaxHealth + 1.f,
		nullptr,
		this,
		UDamageType::StaticClass()
	);
}


void APeCoEnemyCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void APeCoEnemyCharacter::ReceiveDamage(AActor* DamagedActor, float InputDamage, const UDamageType* DamageType, AController* InstigatorController, AActor* DamageCauser)
{
	VarDamageCauser = DamageCauser;
	APeCoGameMode* PeCoGameMode = GetWorld()->GetAuthGameMode<APeCoGameMode>();
	check(PeCoGameMode);
	InputDamage = PeCoGameMode->CalculateDamage(InstigatorController, GetController(), InputDamage);

	float DamageToHealth = InputDamage;


	if (DamageToHealth > 0)
	{
		ShowFloatingText(this, InstigatorController, Damage);
	}

	Health = FMath::Clamp(Health - DamageToHealth, 0.f, MaxHealth);
	if (Health <= 0.f)
	{
		//Flamethrower Wreckage
		if (DamageCauser && DamageCauser->IsA(AFlamethrower::StaticClass()))
		{
			AFlamethrower* Flamethrower = Cast<AFlamethrower>(DamageCauser);
			if (Flamethrower && Flamethrower->HasWeaponEvolved())
			{
				Flamethrower->SpawnWreckage(GetActorLocation());
				DropItem(true);
			}
			else
			{
				// 화염방사기(미진화) 사망 경로: 풀로 반환.
				ReleaseSelfToPool();
			}
		}
		else
		{
			DropItem(false);
		}
		//End of Flamethrower Wreckage

		if (IsValid(InstigatorController))
		{
			APeCoPlayerState* PS = InstigatorController->GetPlayerState<APeCoPlayerState>();
			if (IsValid(PS))
			{
				PS->AddToKillCount(1);
			}
		}
	
		GameOver();
	}
}

void APeCoEnemyCharacter::GameOver()
{
	APeCoGameMode* PeCoGameMode = GetWorld()->GetAuthGameMode<APeCoGameMode>();
	// To Do : PeCoGameMode -> EnemyEliminated
	GetWorld()->GetTimerManager().ClearAllTimersForObject(this);
	// Destroy 대신 풀로 반환. 풀이 없는 경우(비 PIE/비정상 상태)에는 ReleaseSelfToPool 내부에서 Destroy로 폴백.
	ReleaseSelfToPool();
}


void APeCoEnemyCharacter::ResetSlowStatus()
{
	bIsSlowed = false;
}

void APeCoEnemyCharacter::ResetStunStatus()
{
	bIsStun = false;
}

float APeCoEnemyCharacter::GetMaxHealth()
{
	return MaxHealth;
}

void APeCoEnemyCharacter::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if (OtherActor && OtherActor != this && OtherActor->IsA(APeCoPlayerCharacter::StaticClass()))
	{
		// Apply physical force when colliding with the player
		FVector KnockbackDirection = GetActorLocation() - OtherActor->GetActorLocation();
		KnockbackDirection.Normalize();

		//Use LaunchCharacter to smoothly knock back
		LaunchCharacter(KnockbackDirection * KnockbackForce, true, true);  // Keep Z-axis velocity

		bRecentlyKnockedBack = true;
				
		GetWorld()->GetTimerManager().SetTimer(KnockbackTimerHandle, this, &APeCoEnemyCharacter::ResetKnockbackFlag, 2.0f, false);  // Prevent re-collision for 2 seconds

					
	}
}

void APeCoEnemyCharacter::ResetKnockbackFlag()
{
	bRecentlyKnockedBack = false;
	 
}

void APeCoEnemyCharacter::ApplyStatsFromData(const FEnemyStats& Stats)
{
	// ������ ���̺���� �� ����
	Health = Stats.Health;
	MaxHealth = Stats.Health;
	Damage = Stats.Damage; // ������ �� �߰� 
	

	// �̵� �ӵ� ����
	if (GetCharacterMovement()->MovementMode == EMovementMode::MOVE_Flying)
	{
		GetCharacterMovement()->MaxFlySpeed = Stats.FlySpeed;
	}
	else
	{
		GetCharacterMovement()->MaxWalkSpeed = Stats.WalkSpeed;
	}
}

void APeCoEnemyCharacter::DropItem(bool bDropFlameSample)
{
	UPeCoGameInstance* GameInstance = CastChecked<UPeCoGameInstance>(GetGameInstance());
	UDataTable* DropTable = GameInstance->EnemyDropTable;
	if (!DropTable)
	{
		UE_LOG(LogTemp, Error, TEXT("몬스터 드랍 데이터 테이블 없음."));
		return;
	}

	if (EnemyTag.MatchesTag(PeCoGameplayTags::Enemy))
	{
		FEnemyDropData* Row = DropTable->FindRow<FEnemyDropData>("Item.Material.BiologicalSample", TEXT("Read Drop table"), false);
		AsyncLoadDropItem(Row);
	}

	if (EnemyTag.MatchesTag(PeCoGameplayTags::Enemy_Spider))
	{
		FEnemyDropData* Row = DropTable->FindRow<FEnemyDropData>("Item.Material.WebSample", TEXT("Read Drop table"), false);
		AsyncLoadDropItem(Row);
	}
	else if (EnemyTag.MatchesTag(PeCoGameplayTags::Enemy_Mosquito))
	{
		FEnemyDropData* Row = DropTable->FindRow<FEnemyDropData>("Item.Material.VirusSample", TEXT("Read Drop table"), false);
		AsyncLoadDropItem(Row);
	}

	if (bDropFlameSample)
	{
		FEnemyDropData* Row = DropTable->FindRow<FEnemyDropData>("Item.Material.FlameBioSample", TEXT("Read Drop table"), false);
		AsyncLoadDropItem(Row);
	}
}

void APeCoEnemyCharacter::AsyncLoadDropItem(const FEnemyDropData* Row)
{
	if (FMath::RandRange(1, 100) <= Row->DropRate)
	{
		if (IsValid(Row->Item.Get()))
		{
			SpawnItem(Row->Item.Get());
		}
		else
		{
			FStreamableManager& Streamable = UAssetManager::GetStreamableManager();
			Streamable.RequestAsyncLoad(Row->Item.ToSoftObjectPath(), FStreamableDelegate::CreateUObject(this, &APeCoEnemyCharacter::SpawnItem, Row->Item.Get()));
		}
	}
	else
	{
		// 드랍 미확정 시에도 풀로 반환하여 재사용.
		ReleaseSelfToPool();
	}
}

void APeCoEnemyCharacter::SpawnItem(UClass* ItemClass)
{
	if (!GetWorld()) return;

	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	SpawnParams.Owner = nullptr;
	SpawnParams.Instigator = nullptr;

	const AActor* ItemCDO = Cast<AActor>(ItemClass->StaticClass()->GetDefaultObject());
	FTransform NewTransform = GetActorTransform();
	NewTransform.SetScale3D(IsValid(ItemCDO) ? ItemCDO->GetActorScale() : FVector::OneVector);
	AActor* NewItemActor = GetWorld()->SpawnActor(ItemClass, &NewTransform, SpawnParams);

	// 드랍 완료 후 풀로 반환.
	ReleaseSelfToPool();
}

void APeCoEnemyCharacter::ApplyTickDamage(float TickInterval, float DamagePerTick, float Duration, AActor* DamageCauser, AController* InstInstigator)
{
	if (!GetWorld() || TickInterval <= 0.0f || Duration <= 0.0f)
	{
		return;
	}

	int32 TotalTicks = FMath::CeilToInt(Duration / TickInterval);
	int32 CurrentTick = 0;

	FTimerHandle TickDamageTimerHandle;

	GetWorld()->GetTimerManager().SetTimer(
		TickDamageTimerHandle,
		FTimerDelegate::CreateWeakLambda(this, [=, this]() mutable
			{
				if (CurrentTick >= TotalTicks)
				{
					GetWorld()->GetTimerManager().ClearTimer(TickDamageTimerHandle);
					return;
				}

				UGameplayStatics::ApplyDamage(
					this,
					DamagePerTick,
					InstInstigator,
					DamageCauser,
					UDamageType::StaticClass()
				);

				UE_LOG(LogTemp, Warning, TEXT("틱 데미지: %s에게 %f 데미지 적용 (Tick: %d/%d)"),
					*GetName(), DamagePerTick, CurrentTick + 1, TotalTicks);

				++CurrentTick;

			}),
		TickInterval,
		true 
	);

	ALarvaLauncher* LarvaLauncherWeapon = Cast<ALarvaLauncher>(DamageCauser);
	if (LarvaLauncherWeapon)
	{
		bIsWithered = false;
	}

	AFlamethrower* FlamethrowerWeapon = Cast<AFlamethrower>(DamageCauser);
	if (LarvaLauncherWeapon)
	{
		bIsBurned = false;
	}

}

void APeCoEnemyCharacter::RemoveSpawnImmunity()
{
	bIsImmune = false; // 무적 상태 해제
	UE_LOG(LogTemp, Log, TEXT("Spawn immunity removed for %s"), *GetName());
}

// ---------------- IPoolable ----------------

void APeCoEnemyCharacter::OnAcquired_Implementation(const FTransform& SpawnTransform, AActor* NewOwner, APawn* NewInstigator)
{
	// 1) HP/스탯 초기화. ApplyStatsFromData가 이후에 호출되면 덮어써진다.
	Health = MaxHealth;

	// 2) 상태 플래그 초기화.
	bRecentlyKnockedBack = false;
	bIsSlowed = false;
	bIsStun = false;
	bIsWithered = false;
	bIsBurned = false;

	// 3) 이전 인스턴스의 잔여 타이머 정리 (무적/넉백/틱데미지 등).
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearAllTimersForObject(this);
	}

	// 4) Movement 정리. Velocity 잔존 방지. Walking 모드로 복구 (FlyingEnemy는 자식에서 Flying으로 덮어씀).
	if (UCharacterMovementComponent* Move = GetCharacterMovement())
	{
		Move->StopMovementImmediately();
		Move->Velocity = FVector::ZeroVector;
		Move->SetMovementMode(EMovementMode::MOVE_Walking);
	}

	// 4-1) 콜리전 복구. SetActorEnableCollision은 풀 대기 중 채널 응답을 저장/복원하지만,
	//      Character의 Capsule/Mesh에서 Projectile Overlap이 재사용 시 감지되지 않는 이슈가 있어
	//      Enabled 상태만 명시적으로 재적용. 채널 응답은 BP의 CDO 값이 유지되도록 건드리지 않는다.
	if (UCapsuleComponent* Capsule = GetCapsuleComponent())
	{
		Capsule->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	}
	if (USkeletalMeshComponent* SkelMesh = GetMesh())
	{
		SkelMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	}

	// 5) AI 재바인딩. 풀에서 꺼낸 경우 Controller가 남아있을 수 있으므로 다시 Possess.
	//    Boss처럼 AutoPossessAI=PlacedInWorldOrSpawned 설정이어도 재사용 경로에서는 자동 Possess가 타지 않는다.
	if (AController* ExistingController = GetController())
	{
		ExistingController->UnPossess();
	}
	SpawnDefaultController();

	// 6) 스폰 무적 재가동.
	bIsImmune = true;
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().SetTimer(
			SpawnImmunityTimerHandle,
			this,
			&APeCoEnemyCharacter::RemoveSpawnImmunity,
			SpawnImmunityTime,
			false);
	}

	UE_LOG(LogTemp, Verbose, TEXT("[Pool] Enemy OnAcquired: %s at %s"), *GetName(), *SpawnTransform.GetLocation().ToString());
}

void APeCoEnemyCharacter::OnReleased_Implementation()
{
	// 1) 모든 타이머 정리. (틱 데미지 람다 포함)
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearAllTimersForObject(this);
	}

	// 2) 이동 중단. ParkingLocation(Z=-100000)에서 중력/비행 업데이트가 돌지 않도록 MovementMode=None.
	if (UCharacterMovementComponent* Move = GetCharacterMovement())
	{
		Move->StopMovementImmediately();
		Move->Velocity = FVector::ZeroVector;
		Move->SetMovementMode(EMovementMode::MOVE_None);
	}

	// 3) AIController UnPossess — 풀 대기 중 BT/Blackboard가 계속 도는 것을 방지.
	if (AController* Ctrl = GetController())
	{
		Ctrl->UnPossess();
	}

	UE_LOG(LogTemp, Verbose, TEXT("[Pool] Enemy OnReleased: %s"), *GetName());
}

void APeCoEnemyCharacter::ReleaseSelfToPool()
{
	if (UWorld* World = GetWorld())
	{
		if (UPeCoPoolSubsystem* Pool = World->GetSubsystem<UPeCoPoolSubsystem>())
		{
			Pool->ReleaseActor(this);
			return;
		}
	}
	// 폴백: 풀 서브시스템이 없으면 기존 동작 유지.
	Destroy();
}
