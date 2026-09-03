// Fill out your copyright notice in the Description page of Project Settings.


#include "07_Weapon/Projectile.h"
#include "07_Weapon/Weapon.h"
#include "07_Weapon/ProjectileWeapon/LarvaLauncher.h"
#include "07_Weapon/ProjectileWeapon/WebRevolver.h"
#include "07_Weapon/ProjectileWeapon/AirGun.h"

#include "01_Character/PeCoEnemyCharacter.h"

#include "20_System/Pool/PeCoPoolSubsystem.h"

#include "Components/StaticMeshComponent.h"
#include "Components/PrimitiveComponent.h"
#include "Components/CapsuleComponent.h"

#include "GameFramework/ProjectileMovementComponent.h"
#include "GameFramework/DamageType.h"

#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"

#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"


// Sets default values
AProjectile::AProjectile()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

    RootCollisionComponent = CreateDefaultSubobject<UCapsuleComponent>(TEXT("Root Collision Component"));
    RootCollisionComponent->InitCapsuleSize(20.0f, 40.0f);
    RootCollisionComponent->SetCollisionProfileName(TEXT("Projectile"));
    RootCollisionComponent->SetGenerateOverlapEvents(true);
    SetRootComponent(RootCollisionComponent);

    //RootSceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root Scene Component"));
	ProjectileMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Projectile Mesh"));
    ProjectileMesh -> SetupAttachment(RootCollisionComponent);

	ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("Projectile Movement Component"));
	ProjectileMovementComponent->MaxSpeed = 1300.f;
	ProjectileMovementComponent->InitialSpeed = 1300.f;

    // Niagara 궤적 컴포넌트 - 풀링 재사용을 위해 멤버 컴포넌트로 보유.
    // System 에셋은 NiagaraTraceEffect(UPROPERTY) 값으로 BeginPlay/OnAcquired에서 주입.
    TraceEffectComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("Trace Effect Component"));
    TraceEffectComponent->SetupAttachment(RootCollisionComponent);
    TraceEffectComponent->bAutoActivate = false;
}

// Called when the game starts or when spawned
void AProjectile::BeginPlay()
{
	Super::BeginPlay();

    // 콜리전 델리게이트는 인스턴스당 최초 1회만 바인딩.
    BindCollisionDelegates();

    // 비 풀 경로(GetWorld()->SpawnActor 직접 호출)로 생성된 경우에만 기존 동작을 복원한다.
    // 풀 경로는 Pool->ActivateActor가 위치/Owner 확정 후 OnAcquired를 다시 호출해주므로
    // BeginPlay에서는 초기화하지 않는다. (PreWarm으로 ParkingLocation에 생성되었을 때
    //  StartLocation이 -100000로 오염되는 것을 방지)
    if (GetOwner() != nullptr)
    {
        OnAcquired_Implementation(GetActorTransform(), GetOwner(), GetInstigator());
    }
}

void AProjectile::BindCollisionDelegates()
{
    if (bDelegatesBound || !RootCollisionComponent)
    {
        return;
    }
    RootCollisionComponent->OnComponentHit.AddDynamic(this, &AProjectile::OnHit);
    RootCollisionComponent->OnComponentBeginOverlap.AddDynamic(this, &AProjectile::OnBeginOverlap);
    bDelegatesBound = true;
}

void AProjectile::OnAcquired_Implementation(const FTransform& SpawnTransform, AActor* NewOwner, APawn* NewInstigator)
{
    // 콜리전 바인딩이 아직이면(풀 PreWarm으로 BeginPlay 이전에 Acquire될 수 있음) 여기서 수행.
    BindCollisionDelegates();

    StartLocation = SpawnTransform.GetLocation();

    // 소유 무기로부터 사거리 재획득.
    AWeapon* OwnerWeapon = Cast<AWeapon>(NewOwner);
    if (OwnerWeapon)
    {
        MaxDistance = OwnerWeapon->GetRange();
    }

    // 콜리전/가시성 복구.
    if (RootCollisionComponent)
    {
        RootCollisionComponent->SetVisibility(true);
        // Blueprint에 저장된 기존 Custom 응답과 관계없이 전용 Profile을 복원한다.
        RootCollisionComponent->SetCollisionProfileName(TEXT("Projectile"));
        RootCollisionComponent->SetGenerateOverlapEvents(true);
    }

    if (ProjectileMesh)
    {
        ProjectileMesh->SetVisibility(true);
    }

    // ProjectileMovement 재시동: 이전 Velocity 잔존 제거 + 진행 방향으로 초기 속도 재주입.
    if (ProjectileMovementComponent)
    {
        ProjectileMovementComponent->StopMovementImmediately();
        // UpdatedComponent가 풀링 Release 단계에서 nullptr로 내려가 있을 때만 재바인딩.
        // 항상 호출하면 내부 상태(PrevLocation 등)가 흔들릴 수 있다.
        if (ProjectileMovementComponent->UpdatedComponent == nullptr)
        {
            ProjectileMovementComponent->SetUpdatedComponent(RootCollisionComponent);
        }
        const FVector Forward = SpawnTransform.GetRotation().GetForwardVector();
        ProjectileMovementComponent->Velocity = Forward * ProjectileMovementComponent->InitialSpeed;
    }

    // Niagara 궤적 이펙트 재활성화 (시스템 에셋이 비어 있으면 아무 것도 하지 않음).
    if (TraceEffectComponent)
    {
        if (NiagaraTraceEffect && TraceEffectComponent->GetAsset() != NiagaraTraceEffect)
        {
            TraceEffectComponent->SetAsset(NiagaraTraceEffect);
        }
        TraceEffectComponent->SetRelativeScale3D(TraceEffectScale);
        if (NiagaraTraceEffect)
        {
            TraceEffectComponent->ResetSystem();
            TraceEffectComponent->Activate(true);
        }
    }
}

void AProjectile::OnReleased_Implementation()
{
    // 이동 정지 - 재활성화 시 OnAcquired에서 재시동.
    if (ProjectileMovementComponent)
    {
        ProjectileMovementComponent->StopMovementImmediately();
        ProjectileMovementComponent->SetUpdatedComponent(nullptr);
    }

    // 콜리전 OFF는 Pool Subsystem에서도 SetActorEnableCollision(false)로 수행하지만,
    // 루트 컴포넌트 레벨도 명시적으로 정리해 OnHit/Overlap 재등록 없이도 안전하게 대기.
    if (RootCollisionComponent)
    {
        RootCollisionComponent->SetVisibility(false);
        RootCollisionComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    }

    // Niagara 궤적 중지.
    if (TraceEffectComponent)
    {
        TraceEffectComponent->Deactivate();
    }

    // 내부 타이머가 있었다면 모두 해제.
    if (UWorld* World = GetWorld())
    {
        World->GetTimerManager().ClearAllTimersForObject(this);
    }
}


// Called every frame
void AProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	float DistanceTravelled = FVector::Dist(GetActorLocation(), StartLocation);

	if (DistanceTravelled >= MaxDistance)
	{
		ReleaseSelf();
	}

}

void AProjectile::SetDamage(float InDamage)
{
	Damage = InDamage;
}

UProjectileMovementComponent* AProjectile::GetProjectileMovementComponent() const
{
	return ProjectileMovementComponent;
}

void AProjectile::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
    // Pawn(Player/Enemy/동료)은 Overlap으로 처리되므로 여기 도달하지 않음.
    // WorldStatic/WorldDynamic(벽, 환경) 충돌 처리.

    if (OtherActor == this || OtherActor == GetOwner() || OtherActor == GetInstigator())
    {
        return;
    }

    RootCollisionComponent->SetVisibility(false);
    RootCollisionComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);

    if (NiagaraImpactEffect)
    {
        UNiagaraFunctionLibrary::SpawnSystemAtLocation(
            GetWorld(),
            NiagaraImpactEffect,
            GetActorLocation(),
            GetActorRotation(),
            ImpactEffectScale
        );
    }

    ReleaseSelf();
}

void AProjectile::OnBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    AActor* MyOwner = GetOwner();
    if (MyOwner == nullptr)
    {
        ReleaseSelf();
        return;
    }

    if (!OtherActor || OtherActor == this || OtherActor == MyOwner)
    {
        return;
    }

    APawn* WeaponOwnerPawn = Cast<APawn>(MyOwner->GetOwner());
    if (OtherActor == WeaponOwnerPawn)
    {
        return;
    }

    APeCoEnemyCharacter* HitEnemy = Cast<APeCoEnemyCharacter>(OtherActor);

    // Enemy가 아닌 Pawn(Player/동료 등)은 관통
    if (!HitEnemy)
    {
        return;
    }

    AController* MyOwnerInstigator = WeaponOwnerPawn ? WeaponOwnerPawn->GetController() : nullptr;

    UGameplayStatics::ApplyDamage(HitEnemy, Damage, MyOwnerInstigator, this, UDamageType::StaticClass());

    ALarvaLauncher* LarvaLauncherWeapon = Cast<ALarvaLauncher>(MyOwner);
    if (LarvaLauncherWeapon)
    {
        LarvaLauncherWeapon->ApplyWitherEffect(HitEnemy);
    }

    AAirGun* AirGunWeapon = Cast<AAirGun>(MyOwner);
    if (AirGunWeapon)
    {
        AirGunWeapon->ApplyStunEffect(HitEnemy);
    }

    AWebRevolver* WebRevolverWeapon = Cast<AWebRevolver>(MyOwner);
    if (WebRevolverWeapon)
    {
        WebRevolverWeapon->ApplySlowEffect(HitEnemy, WebRevolverWeapon->SlowMultiplier);

        if (WebRevolverWeapon->HasWeaponEvolved())
        {
            WebRevolverWeapon->SpawnFragmentProjectiles(GetActorLocation(), GetActorRotation());
        }
    }

    RootCollisionComponent->SetVisibility(false);
    RootCollisionComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);

    if (NiagaraImpactEffect)
    {
        UNiagaraFunctionLibrary::SpawnSystemAtLocation(
            GetWorld(),
            NiagaraImpactEffect,
            GetActorLocation(),
            GetActorRotation(),
            ImpactEffectScale
        );
    }

    ReleaseSelf();
}

void AProjectile::ReleaseSelf()
{
    if (UWorld* World = GetWorld())
    {
        if (UPeCoPoolSubsystem* Pool = World->GetSubsystem<UPeCoPoolSubsystem>())
        {
            Pool->ReleaseActor(this);
            return;
        }
    }
    // 폴백: 풀이 없으면 기존 동작 유지.
    Destroy();
}
