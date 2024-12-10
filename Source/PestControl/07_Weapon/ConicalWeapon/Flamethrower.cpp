// Fill out your copyright notice in the Description page of Project Settings.


#include "07_Weapon/ConicalWeapon/Flamethrower.h"
#include "07_Weapon/WeaponSub/Wreckage.h"

#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h" 


AFlamethrower::AFlamethrower()
{
    PrimaryActorTick.bCanEverTick = true;

    WeaponTag = PeCoGameplayTags::Weapon_Conical_Flamethrower;
    WeaponType = EWeaponType::Conical;

    FlamethrowerParticle = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("FlamethrowerParticle"));
    FlamethrowerParticle->SetupAttachment(BulletSpawnPoint);
    FlamethrowerParticle->bAutoActivate = false;

    //MaxParticleLifetime = 1.2f; 
    //MaxParticleInitialVelocity = FVector(0.0f, 0.0f, 700.0f); 
}

void AFlamethrower::BeginPlay()
{
    Super::BeginPlay();

    DebugColor = FColor::Red;

}

void AFlamethrower::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (FlamethrowerParticle && FlamethrowerParticle->IsActive() && BulletSpawnPoint)
    {
        FVector CurrentLocation = FlamethrowerParticle->GetComponentLocation();
        FVector TargetLocation = BulletSpawnPoint->GetComponentLocation();

        FRotator CurrentRotation = FlamethrowerParticle->GetComponentRotation();
        FRotator TargetRotation = BulletSpawnPoint->GetComponentRotation() + FRotator(-90.0f, 0.0f, 0.0f);

        FVector NewLocation = FMath::VInterpTo(CurrentLocation, TargetLocation, DeltaTime, 10.0f);
        FlamethrowerParticle->SetWorldLocation(NewLocation);

        FRotator NewRotation = FMath::RInterpTo(CurrentRotation, TargetRotation, DeltaTime, 10.0f);
        FlamethrowerParticle->SetWorldRotation(NewRotation);
    }

    if (Ammo == 0)
    {
        FlamethrowerParticle->DeactivateSystem();
    }
}

void AFlamethrower::ApplyBurnDamage(AActor* Target)
{
    if (!IsValid(Target))
    {
        UE_LOG(LogTemp, Warning, TEXT("ApplyBurnDamage: Target is invalid or already destroyed."));
        return;
    }

    UGameplayStatics::ApplyDamage(Target, BurnDamage, GetInstigatorController(), this, nullptr);
}

void AFlamethrower::ApplyBurnEffect(APeCoEnemyCharacter* EnemyCharacter)
{
    if (!EnemyCharacter)
    {
        return;
    }

    if (!EnemyCharacter->bIsBurned)
    {
        EnemyCharacter->bIsBurned = true;

        AController* InstigatorController = nullptr;

        if (APawn* OwnerPawn = Cast<APawn>(GetOwner()))
        {
            InstigatorController = OwnerPawn->GetController();
        }
        EnemyCharacter->ApplyTickDamage(BurnTickInterval, BurnDamage, BurnDuration, this, InstigatorController);
    };
}

void AFlamethrower::SpawnWreckage(FVector Location)
{
    FHitResult HitResult;
    FVector Start = Location + FVector(0.f, 0.f, 50.f);
    FVector End = Location - FVector(0.f, 0.f, 1000.f);

    FCollisionQueryParams TraceParams;
    TraceParams.AddIgnoredActor(this);
    TraceParams.bTraceComplex = true;

    if (GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECC_Visibility, TraceParams))
    {
        Location.Z = HitResult.Location.Z;
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("Failed to find ground for Wreckage spawn. Using default Z."));
    }

    if (AWreckage* Wreckage = GetWorld()->SpawnActor<AWreckage>(WreckageClass, Location, FRotator::ZeroRotator))
    {
        Wreckage->SetOwner(this);
        Wreckage->InitializeWreckage(WreckageDamage, WreckageTickTime, WreckageDuration);
        UE_LOG(LogTemp, Log, TEXT("Wreckage spawned at location: %s"), *Location.ToString());
    }
}


bool AFlamethrower::EnhancementWeapon(int32 EnhancementIndex)
{
    if (Super::EnhancementWeapon(EnhancementIndex) == false) return false;
    FireAngle += 5;

    ////TODO: 진화할때 길어지는 사거리와 각 값 비율 찾기
    //SetMaxParticleLifetime();
    //SetMaxParticleInitialVelocity();

    return true;
}

bool AFlamethrower::EvolveWeapon(int32 EvolveIndex)
{
    if (Super::EvolveWeapon(EvolveIndex) == false) return false;

    FlamethrowerEvolve();
    return true;
}
void AFlamethrower::FlamethrowerEvolve()
{
}

void AFlamethrower::ConicalFire()
{
    if (FlamethrowerParticle)
    {
        FlamethrowerParticle->ActivateSystem();
    }

    Super::ConicalFire();
}

//void AFlamethrower::SetMaxParticleLifetime()
//{
//    if (FlamethrowerParticle)
//    {
//        // 기존 값에 0.3을 추가
//        MaxParticleLifetime += 0.3f;
//
//        // Lifetime.Max 값 업데이트
//        FlamethrowerParticle->SetFloatParameter(FName("Lifetime.Max"), MaxParticleLifetime);
//
//        // Lifetime.Max 값을 가져와 로그로 출력
//        float CurrentLifetimeMax = 0.0f;
//        FlamethrowerParticle->GetFloatParameter(FName("Lifetime.Max"), CurrentLifetimeMax);
//
//        UE_LOG(LogTemp, Log, TEXT("Particle Lifetime.Max updated to: %f"), CurrentLifetimeMax);
//    }
//}
//
//void AFlamethrower::SetMaxParticleInitialVelocity()
//{
//    if (FlamethrowerParticle)
//    {
//        // 기존 값에 (0, 0, 0.3) 추가
//        MaxParticleInitialVelocity += FVector(0.0f, 0.0f, 100.0f);
//
//        // InitialVelocity.Max 값 업데이트
//        FlamethrowerParticle->SetVectorParameter(FName("InitialVelocity.Max"), MaxParticleInitialVelocity);
//
//        // InitialVelocity.Max 값을 가져와 로그로 출력
//        FVector CurrentVelocityMax = FVector::ZeroVector;
//        FlamethrowerParticle->GetVectorParameter(FName("InitialVelocity.Max"), CurrentVelocityMax);
//
//        UE_LOG(LogTemp, Log, TEXT("Particle InitialVelocity.Max updated to: %s"), *CurrentVelocityMax.ToString());
//    }
//}