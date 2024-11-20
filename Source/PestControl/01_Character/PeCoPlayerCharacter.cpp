// Fill out your copyright notice in the Description page of Project Settings.


#include "01_Character/PeCoPlayerCharacter.h"
#include "01_Character/PeCoEnemyCharacter.h"
#include "01_Character/Components/InventoryComponent.h"
#include "01_Character/Components/EquipmentComponent.h"
#include "01_Character/Components/BuffComponent.h"

#include "02_Player/PeCoPlayerController.h"
#include "02_Player/PeCoPlayerState.h"


#include "04_UI/PeCoHUD.h"

#include "07_Weapon/Projectile.h"
#include "07_Weapon/ProjectileWeapon/LarvaLauncher.h"
#include "07_Weapon/ProjectileWeapon/WebRevolver.h"
#include "07_Weapon/ProjectileWeapon/PestShotgun.h"
#include "07_Weapon/ProjectileWeapon/RoachShooter.h"
#include "07_Weapon/ProjectileWeapon/AirGun.h"
#include "07_Weapon/ConicalWeapon/Pesticide.h"
#include "07_Weapon/ConicalWeapon/Flamethrower.h"

#include "09_Items/SprayBomb.h"

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

	WeaponSpawnPoint = CreateDefaultSubobject<USceneComponent>(TEXT("Weapon Spawn Point"));
	WeaponSpawnPoint->SetupAttachment(RootComponent);


	// Activate ticking in order to update the cursor every frame.
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;

	 GetCapsuleComponent()->OnComponentHit.AddDynamic(this, &APeCoPlayerCharacter::OnHit);

	 bIsInvincible = false;

	 InventoryComponent = CreateDefaultSubobject<UInventoryComponent>(TEXT("InventoryComponent"));
	 EquipmentComponent = CreateDefaultSubobject<UEquipmentComponent>(TEXT("EquipmentComponent"));
	 BuffComponent = CreateDefaultSubobject<UBuffComponent>(TEXT("BuffComponent"));
}


void APeCoPlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

	APeCoPlayerState* PeCoPlayerState = Cast<APeCoPlayerState>(GetPlayerState());
	OnTakeAnyDamage.AddDynamic(PeCoPlayerState, &APeCoPlayerState::ReceiveDamage);

	PeCoPlayerController = Cast<APlayerController>(GetController());

	UE_LOG(LogTemp, Warning, TEXT("health: %f"), PeCoPlayerState->Health);

	InitializeWeaponClasses();

	SpawnWeapon("LarvaLauncher");
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

void APeCoPlayerCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	if (BuffComponent)
	{
		if (GetCharacterMovement())
		{
			BuffComponent->SetInitialSpeeds(GetCharacterMovement()->MaxWalkSpeed, GetCharacterMovement()->MaxWalkSpeedCrouched);
		}
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

void APeCoPlayerCharacter::InitializeWeaponClasses()
{
	WeaponClassMap.Add("LarvaLauncher", LarvaLauncherClass);
	WeaponClassMap.Add("WebRevolver", WebRevolverClass);
	WeaponClassMap.Add("PestShotgun", PestShotgunClass);
	WeaponClassMap.Add("RoachShooter", RoachShooterClass);
	WeaponClassMap.Add("AirGun", AirGunClass);
	WeaponClassMap.Add("Pesticide", PesticideClass);
	WeaponClassMap.Add("Flamethrower", FlamethrowerClass);
}

void APeCoPlayerCharacter::SpawnWeapon(FName WeaponName)
{
	if (!WeaponClassMap.Contains(WeaponName) || !WeaponSpawnPoint)
	{
		UE_LOG(LogTemp, Warning, TEXT("Weapon class or spawn point is invalid for %s"), *WeaponName.ToString());
		return;
	}

	TSubclassOf<AWeapon> WeaponClass = WeaponClassMap[WeaponName];
	if (!WeaponClass)
	{
		UE_LOG(LogTemp, Warning, TEXT("No valid weapon class found for %s"), *WeaponName.ToString());
		return;
	}

	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = this;

	AWeapon* SpawnedWeapon = GetWorld()->SpawnActor<AWeapon>(WeaponClass, GetActorLocation(), FRotator::ZeroRotator, SpawnParams);
	if (SpawnedWeapon)
	{
		SpawnedWeapon->AttachToComponent(WeaponSpawnPoint, FAttachmentTransformRules::SnapToTargetIncludingScale);
		SpawnedWeapons.Add(WeaponName, SpawnedWeapon);
		UE_LOG(LogTemp, Log, TEXT("Spawned and attached weapon: %s"), *WeaponName.ToString());
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Failed to spawn weapon: %s"), *WeaponName.ToString());
	}
}

void APeCoPlayerCharacter::OnHit(UPrimitiveComponent* PlayerHitComponent, AActor* EnemyHitActor, UPrimitiveComponent* EnemyHitComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if (!bIsInvincible && EnemyHitActor && EnemyHitActor != this && EnemyHitActor->IsA(APeCoEnemyCharacter::StaticClass()))
	{
		UGameplayStatics::ApplyDamage(this, CrashDamage, Cast<APeCoEnemyCharacter>(EnemyHitActor)->GetController(), EnemyHitActor, nullptr);
		
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

void APeCoPlayerCharacter::SprayBombFire()
{
	FVector startLoc = GetActorLocation(); // 발사 지점
	FVector targetLoc = GetCursorLocation();  // 타겟 지점.
	float arcValue = 0.5f;                       // ArcParam (0.0-1.0)
	FVector outVelocity = FVector::ZeroVector;   // 결과 Velocity
	if (UGameplayStatics::SuggestProjectileVelocity_CustomArc(this, outVelocity, startLoc, targetLoc, GetWorld()->GetGravityZ(), arcValue))
	{
		FPredictProjectilePathParams predictParams(20.0f, startLoc, outVelocity, 1.0f);   // 20: tracing 보여질 프로젝타일 크기, 15: 시물레이션되는 Max 시간(초)
		predictParams.DrawDebugTime = 1.0f;     //디버그 라인 보여지는 시간 (초)
		predictParams.DrawDebugType = EDrawDebugTrace::Type::ForDuration;  // DrawDebugTime 을 지정하면 EDrawDebugTrace::Type::ForDuration 필요.
		predictParams.OverrideGravityZ = GetWorld()->GetGravityZ();
		FPredictProjectilePathResult result;
		UGameplayStatics::PredictProjectilePath(this, predictParams, result);


		FVector SpawnLocation = WeaponSpawnPoint->GetComponentLocation();
		FRotator SpawnRotation = WeaponSpawnPoint->GetComponentRotation();

		ASprayBomb* BombInstance = GetWorld()->SpawnActor<ASprayBomb>(SprayBombClass, SpawnLocation, SpawnRotation);
		
		if (BombInstance && BombInstance->BombMesh) // 널 포인터 확인
		{
			BombInstance->BombMesh->AddImpulse(outVelocity, NAME_None, true); // Impulse 추가
		}

	}

}

FVector APeCoPlayerCharacter::GetCursorLocation()
{
	FVector HitLocation = FVector::ZeroVector;

	if (APeCoPlayerController* PlayerController = Cast<APeCoPlayerController>(GetController()))
	{

		if (PlayerController != nullptr)
		{
			FHitResult HitResult;
			PlayerController->GetHitResultUnderCursor(
				ECollisionChannel::ECC_WorldStatic,
				false,
				HitResult
			);

			HitLocation = HitResult.Location;
		}
	}
	return HitLocation;
}
