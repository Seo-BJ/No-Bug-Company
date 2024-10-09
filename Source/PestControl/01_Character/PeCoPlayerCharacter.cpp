// Fill out your copyright notice in the Description page of Project Settings.


#include "01_Character/PeCoPlayerCharacter.h"
#include "01_Character/PeCoEnemyCharacter.h"

#include "02_Player/PeCoPlayerController.h"
#include "02_Player/PeCoPlayerState.h"

#include "04_UI/PeCoHUD.h"

#include "07_Weapon/Projectile.h"

#include "Components/DecalComponent.h"
#include "Components/CapsuleComponent.h"

#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/SpringArmComponent.h"

#include "Materials/Material.h"
#include "Kismet/GameplayStatics.h"
#include "UObject/ConstructorHelpers.h"
#include "Camera/CameraComponent.h"
#include "Engine/World.h"




APeCoPlayerCharacter::APeCoPlayerCharacter()
{
	// Set size for player capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// Don't rotate character to camera direction
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Rotate character to moving direction
	GetCharacterMovement()->RotationRate = FRotator(0.f, 640.f, 0.f);
	GetCharacterMovement()->bConstrainToPlane = true;
	GetCharacterMovement()->bSnapToPlaneAtStart = true;

	// Create a camera boom...
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->SetUsingAbsoluteRotation(true); // Don't want arm to rotate when character does
	CameraBoom->TargetArmLength = 800.f;
	CameraBoom->SetRelativeRotation(FRotator(-60.f, 0.f, 0.f));
	CameraBoom->bDoCollisionTest = false; // Don't want to pull camera in when it collides with level

	// Create a camera...
	TopDownCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("TopDownCamera"));
	TopDownCameraComponent->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	TopDownCameraComponent->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	// Activate ticking in order to update the cursor every frame.
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;

	FireRate = 2.0f;

	 GetCapsuleComponent()->OnComponentHit.AddDynamic(this, &APeCoPlayerCharacter::OnHit);

	 bIsInvincible = false;
}


void APeCoPlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

	APeCoPlayerState* PeCoPlayerState = Cast<APeCoPlayerState>(GetPlayerState());
	OnTakeAnyDamage.AddDynamic(PeCoPlayerState, &APeCoPlayerState::ReceiveDamage);

	GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &APeCoPlayerCharacter::FireWeapon, FireRate, true);
	PeCoPlayerController = Cast<APlayerController>(GetController());

	UE_LOG(LogTemp, Warning, TEXT("health: %f"), PeCoPlayerState->Health);
}

void APeCoPlayerCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	GetCharacterMovement()->bOrientRotationToMovement = true;

	if (PeCoPlayerController)
	{
		FHitResult HitResult;
		PeCoPlayerController->GetHitResultUnderCursor(
			ECollisionChannel::ECC_Visibility,
			false,
			HitResult);

		RotateAim(HitResult.ImpactPoint);
	}
}


// Called to bind functionality to input
void APeCoPlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

ETeam APeCoPlayerCharacter::GetTeam()
{
	APeCoPlayerState* PeCoPlayerState = Cast<APeCoPlayerState>(GetPlayerState());
	check(PeCoPlayerState);
	return PeCoPlayerState->Team;
}


void APeCoPlayerCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	InitPlayerCharacter();
	APeCoPlayerState* PeCoPlayerState = Cast<APeCoPlayerState>(GetPlayerState());
	if (PeCoPlayerState)
	{
		PeCoPlayerState->SetTeam(ETeam::ET_Player);  // Set Player Team		
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("PlayerState is nullptr in APeCoPlayerCharacter PossessedBy!"));
	}

}


void APeCoPlayerCharacter::InitPlayerCharacter()
{
	APeCoPlayerState* PeCoPS = GetPlayerState<APeCoPlayerState>();
	check(PeCoPS);

	APeCoPlayerController* PeCOPC = Cast<APeCoPlayerController>(GetController());
	check(PeCOPC);

	APeCoHUD* PeCoHUD = Cast<APeCoHUD>(PeCOPC->GetHUD());
	check(PeCoHUD);
	PeCoHUD->InitOverlay(PeCOPC, PeCoPS);

}

void APeCoPlayerCharacter::SetNewFireRate(float NewFireRate)
{
	FireRate = NewFireRate;
	GetWorld()->GetTimerManager().ClearTimer(TimerHandle);
	GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &APeCoCharacter::Fire, FireRate, true);
}

void APeCoPlayerCharacter::FireWeapon()
{

	float SpreadAngle = 15.f;
	FVector Location = ProjectileSpawnPoint->GetComponentLocation();
	FRotator Rotation = ProjectileSpawnPoint->GetComponentRotation();

	if (PestShotgunProjectileClass)
	{
		AProjectile* Projectile = GetWorld()->SpawnActor<AProjectile>(PestShotgunProjectileClass, Location, Rotation);
		if (Projectile)
		{
			Projectile->SetOwner(this);  // Set Owner of Projectile -> PeCoPlayerCharacter
		}
	}

	float NumberOfProjectiles = 5;

	for (int32 i = 0; i < NumberOfProjectiles - 1; ++i)
	{
		FRotator SpreadRotation = Rotation;
		float SpreadOffset = (i - NumberOfProjectiles / 2) * SpreadAngle;
		SpreadRotation.Yaw += SpreadOffset;

		if (PestShotgunProjectileClass)
		{
			AProjectile* SpawnedProjectile = GetWorld()->SpawnActor<AProjectile>(PestShotgunProjectileClass, Location, SpreadRotation);
			if (SpawnedProjectile)
			{
				SpawnedProjectile->SetOwner(this);  // Set Owner of Projectile -> PeCoPlayerCharacter
			}
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Projectile class is null"));
		}
	}
}

void APeCoPlayerCharacter::OnHit(UPrimitiveComponent* PlayerHitComponent, AActor* EnemyHitActor, UPrimitiveComponent* EnemyHitComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if (!bIsInvincible && EnemyHitActor && EnemyHitActor != this && EnemyHitActor->IsA(APeCoEnemyCharacter::StaticClass()))
	{
		UGameplayStatics::ApplyDamage(this, CrashDamage, nullptr, nullptr, nullptr);
		
		BecomeInvincible(CrashInvincibleDuration); 
	}
}

void APeCoPlayerCharacter::BecomeInvincible(float InvincibleDuration)
{
	bIsInvincible = true;
	UE_LOG(LogTemp, Warning, TEXT("Player is now invincible!"));

	GetWorld()->GetTimerManager().SetTimer(InvincibilityTimerHandle, this, &APeCoPlayerCharacter::EndInvincible, InvincibleDuration, false);
}

void APeCoPlayerCharacter::EndInvincible()
{
	bIsInvincible = false;
	UE_LOG(LogTemp, Warning, TEXT("Player is no longer invincible."));
}