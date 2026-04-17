// Fill out your copyright notice in the Description page of Project Settings.


#include "07_Weapon/Projectile.h"
#include "07_Weapon/Weapon.h"
#include "07_Weapon/ProjectileWeapon/LarvaLauncher.h"
#include "07_Weapon/ProjectileWeapon/WebRevolver.h"
#include "07_Weapon/ProjectileWeapon/AirGun.h"

#include "01_Character/PeCoEnemyCharacter.h"

#include "Components/StaticMeshComponent.h"
#include "Components/PrimitiveComponent.h"
#include "Components/CapsuleComponent.h"

#include "GameFramework/ProjectileMovementComponent.h"
#include "GameFramework/DamageType.h"

#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"

#include "Kismet/GameplayStatics.h"


// Sets default values
AProjectile::AProjectile()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

    RootCollisionComponent = CreateDefaultSubobject<UCapsuleComponent>(TEXT("Root Collision Component"));
    RootCollisionComponent->InitCapsuleSize(20.0f, 40.0f);
    SetRootComponent(RootCollisionComponent);

    //RootSceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root Scene Component"));
	ProjectileMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Projectile Mesh"));
    ProjectileMesh -> SetupAttachment(RootCollisionComponent);

	ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("Projectile Movement Component"));
	ProjectileMovementComponent->MaxSpeed = 1300.f;
	ProjectileMovementComponent->InitialSpeed = 1300.f; 
}

// Called when the game starts or when spawned
void AProjectile::BeginPlay()
{
	Super::BeginPlay();

	StartLocation = GetActorLocation();
    AWeapon* OwnerWeapon = Cast<AWeapon>(GetOwner());
    if (OwnerWeapon)
    {
        MaxDistance = OwnerWeapon->GetRange();
    }

    RootCollisionComponent->OnComponentHit.AddDynamic(this, &AProjectile::OnHit);
    RootCollisionComponent->OnComponentBeginOverlap.AddDynamic(this, &AProjectile::OnBeginOverlap);
    
    if (NiagaraTraceEffect)
    {
        FVector SpawnLocation = GetActorLocation();
        FRotator SpawnRotation = GetActorRotation();

        ActiveTraceEffect = UNiagaraFunctionLibrary::SpawnSystemAtLocation(
            GetWorld(),
            NiagaraTraceEffect,
            SpawnLocation,
            SpawnRotation,
            TraceEffectScale
        );
    }
}


// Called every frame
void AProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	float DistanceTravelled = FVector::Dist(GetActorLocation(), StartLocation);

	if (DistanceTravelled >= MaxDistance)
	{
		Destroy();
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

    if (OtherActor == this || OtherActor == GetOwner())
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

    if (ActiveTraceEffect)
    {
        ActiveTraceEffect->DestroyComponent();
    }

    Destroy();
}

void AProjectile::OnBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    AActor* MyOwner = GetOwner();
    if (MyOwner == nullptr)
    {
        Destroy();
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

    if (ActiveTraceEffect)
    {
        ActiveTraceEffect->DestroyComponent();
    }

    Destroy();
}