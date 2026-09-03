// Fill out your copyright notice in the Description page of Project Settings.


#include "01_Character/PeCoEnemyCharacter.h"

#include "PestControl.h"
#include "00_GameModes/PeCoGameMode.h"
#include "01_Character/PeCoPlayerCharacter.h"
#include "02_Player/PeCoPlayerState.h"
#include "07_Weapon/ConicalWeapon/Flamethrower.h"
#include "07_Weapon/ProjectileWeapon/LarvaLauncher.h"
#include "10_Enemy/PeCoEnemyAIController.h"
#include "20_System/PeCoGameInstance.h"
#include "20_System/Pool/PeCoPoolSubsystem.h"
#include "21_Data/PeCoDataRow.h"

#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BrainComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Navigation/PathFollowingComponent.h"

#include "Engine/StreamableManager.h"
#include "Engine/AssetManager.h"
#include "GameFramework/DamageType.h"

namespace
{
void SuspendGenericAIControllerForPooling(AAIController& Controller)
{
	Controller.StopMovement();
	Controller.ClearFocus(EAIFocusPriority::Gameplay);

	if (UBrainComponent* Brain = Controller.GetBrainComponent())
	{
		if (UBehaviorTreeComponent* BehaviorTree = Cast<UBehaviorTreeComponent>(Brain))
		{
			BehaviorTree->StopTree(EBTStopMode::Forced);
		}
		else
		{
			Brain->StopLogic(TEXT("Enemy returned to pool"));
		}
		Brain->SetComponentTickEnabled(false);
	}
	if (UPathFollowingComponent* PathFollowing = Controller.GetPathFollowingComponent())
	{
		PathFollowing->SetComponentTickEnabled(false);
	}

	Controller.SetActorTickEnabled(false);
}

void ResumeGenericAIControllerFromPooling(AAIController& Controller)
{
	Controller.SetActorTickEnabled(true);

	if (UBlackboardComponent* Blackboard = Controller.GetBlackboardComponent())
	{
		Blackboard->ClearValue(TEXT("PlayerLocation"));
		Blackboard->ClearValue(TEXT("LastKnownPlayerLocation"));
		if (const APawn* ControlledPawn = Controller.GetPawn())
		{
			Blackboard->SetValueAsVector(TEXT("StartLocation"), ControlledPawn->GetActorLocation());
		}
	}
	if (UPathFollowingComponent* PathFollowing = Controller.GetPathFollowingComponent())
	{
		PathFollowing->SetComponentTickEnabled(true);
	}
	if (UBrainComponent* Brain = Controller.GetBrainComponent())
	{
		Brain->SetComponentTickEnabled(true);
		if (!Brain->IsRunning())
		{
			Brain->RestartLogic();
		}
	}
}
}

APeCoEnemyCharacter::APeCoEnemyCharacter(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.TickGroup = TG_PrePhysics;

	// 풀링 사용/미사용 모두 엔진 기본 TickGroup을 그대로 사용한다.
	// (AIController=TG_PrePhysics, CMC=TG_PrePhysics, Mesh=컴포넌트 기본값)

	// Set KnockbackForce
	KnockbackForce = 1000.0f;

	// Set up Hit event
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_GameTraceChannel1, ECR_Overlap);
	GetCapsuleComponent()->SetGenerateOverlapEvents(true);
	GetMesh()->SetCollisionResponseToChannel(ECC_GameTraceChannel1, ECR_Overlap);
	GetCapsuleComponent()->OnComponentHit.AddDynamic(this, &APeCoEnemyCharacter::OnHit);
	EnemyID = NAME_None;

	MaxHealth = 100.0f; // set max health
	Health = MaxHealth; // when the game start, set health = max health

	// 기본값 초기화
	bIsImmune = true; // 스폰 시 기본적으로 무적 상태
}

void APeCoEnemyCharacter::BeginPlay()
{
	Super::BeginPlay();

	// Blueprint의 기존 Custom 설정보다 Enemy → Projectile Overlap 규칙을 우선한다.
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_GameTraceChannel1, ECR_Overlap);
	GetCapsuleComponent()->SetGenerateOverlapEvents(true);
	GetMesh()->SetCollisionResponseToChannel(ECC_GameTraceChannel1, ECR_Overlap);

	bInitialActorTickEnabled = IsActorTickEnabled();
	CaptureInitialPoolComponentState();
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

}

void APeCoEnemyCharacter::CaptureInitialPoolComponentState()
{
	if (bInitialPoolComponentStateCaptured)
	{
		return;
	}

	if (const UCapsuleComponent* Capsule = GetCapsuleComponent())
	{
		InitialCapsuleCollisionEnabled = Capsule->GetCollisionEnabled();
		bInitialCapsuleGenerateOverlapEvents = Capsule->GetGenerateOverlapEvents();
	}
	if (const USkeletalMeshComponent* SkelMesh = GetMesh())
	{
		InitialMeshCollisionEnabled = SkelMesh->GetCollisionEnabled();
		bInitialMeshGenerateOverlapEvents = SkelMesh->GetGenerateOverlapEvents();
	}

	bInitialPoolComponentStateCaptured = true;
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
		// TODO: 아이템 드랍 경로(async load 재진입/중복 Release 이슈) 추후 수정.
		// 현재는 풀링 안정성을 위해 드랍/Wreckage 경로 비활성화.
		// if (DamageCauser && DamageCauser->IsA(AFlamethrower::StaticClass()))
		// {
		// 	AFlamethrower* Flamethrower = Cast<AFlamethrower>(DamageCauser);
		// 	if (Flamethrower && Flamethrower->HasWeaponEvolved())
		// 	{
		// 		Flamethrower->SpawnWreckage(GetActorLocation());
		// 		DropItem(true);
		// 	}
		// 	else
		// 	{
		// 		ReleaseSelfToPool();
		// 	}
		// }
		// else
		// {
		// 	// DropItem(false);
		// }
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
	Health = Stats.Health;
	MaxHealth = Stats.Health;
	Damage = Stats.Damage; 
	
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

// TODO: async load 콜백 재진입 + 중복 Release 이슈로 추후 재설계. 현재 비활성.
void APeCoEnemyCharacter::AsyncLoadDropItem(const FEnemyDropData* Row)
{
	// if (FMath::RandRange(1, 100) <= Row->DropRate)
	// {
	// 	if (IsValid(Row->Item.Get()))
	// 	{
	// 		SpawnItem(Row->Item.Get());
	// 	}
	// 	else
	// 	{
	// 		FStreamableManager& Streamable = UAssetManager::GetStreamableManager();
	// 		Streamable.RequestAsyncLoad(Row->Item.ToSoftObjectPath(), FStreamableDelegate::CreateUObject(this, &APeCoEnemyCharacter::SpawnItem, Row->Item.Get()));
	// 	}
	// }
	// else
	// {
	// 	ReleaseSelfToPool();
	// }
}
void APeCoEnemyCharacter::SpawnItem(UClass* ItemClass)
{
	// if (!GetWorld()) return;
	//
	// FActorSpawnParameters SpawnParams;
	// SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	// SpawnParams.Owner = nullptr;
	// SpawnParams.Instigator = nullptr;
	//
	// const AActor* ItemCDO = Cast<AActor>(ItemClass->StaticClass()->GetDefaultObject());
	// FTransform NewTransform = GetActorTransform();
	// NewTransform.SetScale3D(IsValid(ItemCDO) ? ItemCDO->GetActorScale() : FVector::OneVector);
	// AActor* NewItemActor = GetWorld()->SpawnActor(ItemClass, &NewTransform, SpawnParams);
	//
	// ReleaseSelfToPool();
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
}

// ---------------- IPoolable ----------------

void APeCoEnemyCharacter::OnAcquired_Implementation(const FTransform& SpawnTransform, AActor* NewOwner, APawn* NewInstigator)
{
	// 정상 Spawn에서는 BeginPlay에서 이미 캡처된다. Deferred/특수 생성 경로도
	// 풀에 들어가기 전의 활성 상태를 보존할 수 있도록 안전망으로 한 번 더 확인한다.
	CaptureInitialPoolComponentState();

	// 상태 초기화. (Health/MaxHealth 등 수치는 Spawner가 이어서 호출하는
	// ApplyStatsFromData에서 일괄 설정하므로 여기서 중복 초기화하지 않는다.
	bRecentlyKnockedBack = false;
	bIsSlowed = false;
	bIsStun = false;
	bIsWithered = false;
	bIsBurned = false;

	// Movement Component 복원 및 Tick 재개.
	if (UCharacterMovementComponent* Move = GetCharacterMovement())
	{
		if (Move->UpdatedComponent == nullptr)
		{
			Move->SetUpdatedComponent(GetCapsuleComponent());
		}
		Move->SetComponentTickEnabled(true);
		Move->StopMovementImmediately();
		Move->Velocity = FVector::ZeroVector;
		if (const UCharacterMovementComponent* CDOMove = GetClass()->GetDefaultObject<ACharacter>()->GetCharacterMovement())
		{
			Move->SetMovementMode(CDOMove->DefaultLandMovementMode);
		}
	}

	if (UCapsuleComponent* Capsule = GetCapsuleComponent())
	{
		Capsule->SetComponentTickEnabled(true);
		Capsule->SetCollisionEnabled(InitialCapsuleCollisionEnabled);
		Capsule->SetGenerateOverlapEvents(bInitialCapsuleGenerateOverlapEvents);
	}

	if (USkeletalMeshComponent* SkelMesh = GetMesh())
	{
		SkelMesh->SetComponentTickEnabled(true);
		SkelMesh->bNoSkeletonUpdate = false;
		SkelMesh->bPauseAnims = false;
		SkelMesh->SetCollisionEnabled(InitialMeshCollisionEnabled);
		SkelMesh->SetGenerateOverlapEvents(bInitialMeshGenerateOverlapEvents);
	}

	// 최초 생성 때 붙은 AIController를 계속 재사용한다.
	AController* Ctrl = GetController();
	if (!Ctrl)
	{
		// 예외 복구 경로. 정상적인 풀 재사용에서는 기존 Controller가 계속 Possess 중이다.
		SpawnDefaultController();
		Ctrl = GetController();
	}
	else if (Ctrl->GetPawn() != this)
	{
		Ctrl->Possess(this);
	}
	if (Ctrl)
	{
		if (APeCoEnemyAIController* EnemyCtrl = Cast<APeCoEnemyAIController>(Ctrl))
		{
			EnemyCtrl->ResumeFromPooling();
		}
		else if (AAIController* AIController = Cast<AAIController>(Ctrl))
		{
			// Ground/Flying Controller는 APeCoEnemyAIController를 상속하지 않는다.
			ResumeGenericAIControllerFromPooling(*AIController);
		}
		else
		{
			Ctrl->SetActorTickEnabled(true);
		}
	}

	// PoolSubsystem은 범용 활성화 과정에서 Actor Tick을 켠다. Enemy는 일반 Spawn 시
	// Actor Tick이 꺼져 있으므로 최초 런타임 상태로 되돌려 ON/OFF 실행 상태를 맞춘다.
	SetActorTickEnabled(bInitialActorTickEnabled);

	// 스폰 무적
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
}
void APeCoEnemyCharacter::OnReleased_Implementation()
{
	// 모든 타이머 정리.
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearAllTimersForObject(this);
	}

	// CharacterMovement 정지 및 Tick 차단.
	if (UCharacterMovementComponent* Move = GetCharacterMovement())
	{
		Move->StopMovementImmediately();
		Move->Velocity = FVector::ZeroVector;
		Move->SetMovementMode(EMovementMode::MOVE_None);
		Move->SetComponentTickEnabled(false);
		Move->SetUpdatedComponent(nullptr);
	}

	if (USkeletalMeshComponent* SkelMesh = GetMesh())
	{
		SkelMesh->SetComponentTickEnabled(false);
		SkelMesh->bNoSkeletonUpdate = true;
		SkelMesh->bPauseAnims = true;
		SkelMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		SkelMesh->SetGenerateOverlapEvents(false);
	}

	if (UCapsuleComponent* Capsule = GetCapsuleComponent())
	{
		Capsule->SetComponentTickEnabled(false);
		Capsule->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		Capsule->SetGenerateOverlapEvents(false);
	}
	// Possess 관계를 유지한 채 AI 실행만 정지한다. UnPossess하면 다음 Acquire에서
	// SpawnDefaultController가 새 Controller를 만들고 기존 Controller가 월드에 누적된다.
	if (AController* Ctrl = GetController())
	{
		if (APeCoEnemyAIController* EnemyCtrl = Cast<APeCoEnemyAIController>(Ctrl))
		{
			EnemyCtrl->SuspendForPooling();
		}
		else if (AAIController* AIController = Cast<AAIController>(Ctrl))
		{
			// 실제 Ground/Flying Controller의 Brain/Path Tick도 함께 정지한다.
			SuspendGenericAIControllerForPooling(*AIController);
		}
		else
		{
			Ctrl->SetActorTickEnabled(false);
		}
	}
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
	Destroy();
}
