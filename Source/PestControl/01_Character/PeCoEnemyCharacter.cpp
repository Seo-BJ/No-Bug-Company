// Fill out your copyright notice in the Description page of Project Settings.


#include "01_Character/PeCoEnemyCharacter.h"

#include "00_GameModes/PeCoGameMode.h"
#include "01_Character/PeCoPlayerCharacter.h"
#include "02_Player/PeCoPlayerState.h"
#include "07_Weapon/ConicalWeapon/Flamethrower.h"
#include "07_Weapon/ProjectileWeapon/LarvaLauncher.h"
#include "20_System/PeCoGameInstance.h"
#include "21_Data/PeCoDataRow.h"

#include "Components/CapsuleComponent.h"
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
				Destroy();
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
	//Destroy();
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
		Destroy();
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

	Destroy();
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
