// Fill out your copyright notice in the Description page of Project Settings.


#include "07_Weapon/WeaponSub/AirGunEvolvedBullet.h"
#include "07_Weapon/ProjectileWeapon/AirGun.h"

#include "Components/CapsuleComponent.h"
#include "01_Character/PeCoEnemyCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"

AAirGunEvolvedBullet::AAirGunEvolvedBullet()
{
    OverlapCapsule = CreateDefaultSubobject<UCapsuleComponent>(TEXT("Overlap Capsule"));
    OverlapCapsule->InitCapsuleSize(50.f, 100.f); 
    OverlapCapsule->SetCollisionProfileName(TEXT("OverlapAllDynamic"));
    OverlapCapsule->SetCollisionObjectType(ECC_GameTraceChannel1);
    OverlapCapsule->SetCollisionResponseToChannel(ECC_GameTraceChannel1, ECR_Ignore);
    OverlapCapsule->SetupAttachment(RootComponent);

    OverlapCapsule->OnComponentBeginOverlap.AddDynamic(this, &AAirGunEvolvedBullet::OnOverlap);

    bHasDealtDamage = false; 
}

void AAirGunEvolvedBullet::BeginPlay()
{
    Super::BeginPlay();

    OverlapCapsule->SetCollisionObjectType(ECC_GameTraceChannel1);
    OverlapCapsule->SetCollisionResponseToChannel(ECC_GameTraceChannel1, ECR_Ignore);
}

void AAirGunEvolvedBullet::OnOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    AActor* MyOwner = GetOwner();
    if (!MyOwner || !OtherActor || OtherActor == this || OtherActor == MyOwner)
    {
        return;
    }

    APeCoEnemyCharacter* HitEnemy = Cast<APeCoEnemyCharacter>(OtherActor);
    if (HitEnemy)
    {
        if (!bHasDealtDamage)
        {
            UGameplayStatics::ApplyDamage(HitEnemy, Damage, MyOwner->GetInstigatorController(), this, UDamageType::StaticClass());
            bHasDealtDamage = true; 
        }

 
        FVector PlayerLocation = MyOwner->GetActorLocation(); 
        FVector EnemyLocation = HitEnemy->GetActorLocation(); 

        FVector KnockbackDirection = EnemyLocation - PlayerLocation; 
        KnockbackDirection.Normalize();

        float KnockbackForce = 1500.0f;
        HitEnemy->LaunchCharacter(KnockbackDirection * KnockbackForce, true, true);


        AAirGun* AirGun = Cast<AAirGun>(MyOwner);
        if (AirGun)
        {
            AirGun->ApplyStunEffect(HitEnemy);
        }
    }
}
