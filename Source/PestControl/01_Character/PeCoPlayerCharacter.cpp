// Fill out your copyright notice in the Description page of Project Settings.


#include "PeCoPlayerCharacter.h"

#include "01_Character/PeCoEnemyCharacter.h"
#include "01_Character/Components/InventoryComponent.h"
#include "01_Character/Components/EquipmentComponent.h"

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
#include "09_Items/AntiSpray.h"

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

	 bIsCrashInvincible = false;

	 SetCanBeDamaged(true);

	 InventoryComponent = CreateDefaultSubobject<UInventoryComponent>(TEXT("InventoryComponent"));
	 EquipmentComponent = CreateDefaultSubobject<UEquipmentComponent>(TEXT("EquipmentComponent"));
}


void APeCoPlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

	APeCoPlayerState* PeCoPlayerState = Cast<APeCoPlayerState>(GetPlayerState());
	OnTakeAnyDamage.AddDynamic(PeCoPlayerState, &APeCoPlayerState::ReceiveDamage);

	PeCoPlayerController = Cast<APlayerController>(GetController());

	InitializeWeaponClasses();

	SpawnWeapon("LarvaLauncher");
}

void APeCoPlayerCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (PeCoPlayerController && WeaponSpawnPoint)
	{
		FHitResult HitResult;
		bool bHit = PeCoPlayerController->GetHitResultUnderCursor(
			ECollisionChannel::ECC_Visibility,
			true,
			HitResult
		);

		FVector TargetLocation = FVector::ZeroVector;

		if (bHit)
		{
			TargetLocation = HitResult.ImpactPoint;
		}
		MoveWeaponSpawnPoint(TargetLocation);
	}
}

void APeCoPlayerCharacter::MoveWeaponSpawnPoint(FVector MouseLocation)
{
	FVector CharacterLocation = GetActorLocation();
	FVector DirectionToMouse = (MouseLocation - CharacterLocation).GetSafeNormal();

	FVector TargetLocation = CharacterLocation + DirectionToMouse ;

	FVector FinalLocation = FMath::ClosestPointOnLine(CharacterLocation, TargetLocation, MouseLocation);

	WeaponSpawnPoint->SetWorldLocation(FinalLocation);

	RotateAim(MouseLocation);
}

void APeCoPlayerCharacter::RotateAim(FVector LookAtTarget)
{
	FVector ToTarget = LookAtTarget - WeaponSpawnPoint->GetComponentLocation();
	FRotator LookAtRotation = FRotator(0.f, ToTarget.Rotation().Yaw, 0.f);

	WeaponSpawnPoint->SetWorldRotation(LookAtRotation);
}

void APeCoPlayerCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();
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
	PeCoPS->InitPlayerStat();
	if (UCharacterMovementComponent* MovementComponent = GetCharacterMovement())
	{
		GetCharacterMovement()->MaxWalkSpeed = PeCoPS->GetMoveSpeed();
	}

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
		AController* PlayerController = GetController();  // PlayerCharacter의 Controller 가져오기
		if (PlayerController)
		{
			SpawnedWeapon->SetInstigator(this);  // 무기의 InstigatorController 설정
		}
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
	if (!bIsCrashInvincible && EnemyHitActor && EnemyHitActor != this && EnemyHitActor->IsA(APeCoEnemyCharacter::StaticClass()))
	{
		// 적 캐릭터로 캐스팅
		APeCoEnemyCharacter* Enemy = Cast<APeCoEnemyCharacter>(EnemyHitActor);
		if (Enemy)
		{
			float EnemyDamage = Enemy->Damage; // 적의 데미지 가져오기
			UGameplayStatics::ApplyDamage(this, EnemyDamage, Enemy->GetController(), Enemy, nullptr);

			// 플레이어가 일정 시간 동안 무적 상태가 되도록 설정
			BecomeCrashInvincible(CrashInvincibleDuration);
		}
		
		/*UGameplayStatics::ApplyDamage(this, CrashDamage, Cast<APeCoEnemyCharacter>(EnemyHitActor)->GetController(), EnemyHitActor, nullptr);
		
		BecomeCrashInvincible(CrashInvincibleDuration);*/
	}
}

void APeCoPlayerCharacter::BecomeCrashInvincible(float Duration)
{
	bIsCrashInvincible = true;
	UE_LOG(LogTemp, Warning, TEXT("Player is now invincible!"));

	GetWorld()->GetTimerManager().SetTimer(CrashInvincibilityTimerHandle, this, &APeCoPlayerCharacter::EndCrashInvincible, CrashInvincibleDuration, false);
}

void APeCoPlayerCharacter::EndCrashInvincible()
{
	bIsCrashInvincible = false;
	UE_LOG(LogTemp, Warning, TEXT("Player is no longer invincible."));
}

//~ Spray Bomb Test
void APeCoPlayerCharacter::SprayBombFire()
{
	FVector startLoc = GetActorLocation(); 
	FVector targetLoc = GetCursorLocation(); 
	float arcValue = 0.5f;                       // ArcParam (0.0-1.0)
	FVector outVelocity = FVector::ZeroVector;  

	float Distance = FVector::Dist(GetActorLocation(), targetLoc);
	
	if(Distance <= BombRange)
	{ 
		if (UGameplayStatics::SuggestProjectileVelocity_CustomArc(this, outVelocity, startLoc, targetLoc, GetWorld()->GetGravityZ(), arcValue))
		{
			FPredictProjectilePathParams predictParams(20.0f, startLoc, outVelocity, 1.0f);  
			predictParams.DrawDebugType = EDrawDebugTrace::Type::ForDuration;  
			predictParams.OverrideGravityZ = GetWorld()->GetGravityZ();
			FPredictProjectilePathResult result;
			UGameplayStatics::PredictProjectilePath(this, predictParams, result);


			FVector SpawnLocation = WeaponSpawnPoint->GetComponentLocation();
			FRotator SpawnRotation = WeaponSpawnPoint->GetComponentRotation();

			ASprayBomb* BombInstance = GetWorld()->SpawnActor<ASprayBomb>(SprayBombClass, SpawnLocation, SpawnRotation);
		
			if (BombInstance && BombInstance->BombMesh) 
			{
				BombInstance->BombMesh->AddImpulse(outVelocity, NAME_None, true);
			}
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
//~ End of Spray Bomb Test

// ~ Anti Spray Test
void APeCoPlayerCharacter::UseAntiSpray()
{
	
	FVector SpawnLocation = WeaponSpawnPoint->GetComponentLocation();
	FRotator SpawnRotation = WeaponSpawnPoint->GetComponentRotation();

	AAntiSpray* AntiSprayInstance = GetWorld()->SpawnActor<AAntiSpray>(AntiSprayClass, SpawnLocation, SpawnRotation);
	
	AntiSprayInstance->ActivateItem(this);
}


void APeCoPlayerCharacter::ActivateInvincibility(float Duration)
{

	SetCanBeDamaged(false);
	UE_LOG(LogTemp, Log, TEXT("Invincibility activated: CanBeDamaged = false"));


	GetWorldTimerManager().SetTimer(InvincibilityTimerHandle, this, &APeCoPlayerCharacter::DeactivateInvincibility, Duration, false);
}

void APeCoPlayerCharacter::DeactivateInvincibility()
{
	SetCanBeDamaged(true);
	UE_LOG(LogTemp, Log, TEXT("Invincibility deactivated: CanBeDamaged = true"));
}
// ~End of  Anti Spray Test
