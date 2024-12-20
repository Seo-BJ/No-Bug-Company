// Fill out your copyright notice in the Description page of Project Settings.


#include "PeCoPlayerCharacter.h"

#include "00_GameModes/PeCoGameMode.h"
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

#include "20_System/PeCoGameInstance.h"

#include "Animation/AnimInstance.h"

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

#include "Materials/MaterialInstanceDynamic.h" // For UMaterialInstanceDynamic
#include "Kismet/KismetMaterialLibrary.h" // For CreateDynamicMaterialInstance
#include "TimerManager.h" // For FTimerHandle




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
	//WeaponSpawnPoint->SetupAttachment(RootComponent);

	SecondWeaponSpawnPoint = CreateDefaultSubobject<USceneComponent>(TEXT("SecondWeapon Spawn Point"));
	SecondWeaponSpawnPoint->SetupAttachment(WeaponSpawnPoint);


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

	UPeCoGameInstance* GameInstance = GetGameInstance<UPeCoGameInstance>();
	if (IsValid(GameInstance))
	{
		SpawnWeapon(GameInstance->SelectedWeaponTag);
		
	}

	APeCoGameMode* PeCoGameMode = Cast<APeCoGameMode>(UGameplayStatics::GetGameMode(this));
	if (PeCoGameMode)
	{
		if (PeCoGameMode->StageNumber > 1)
		{
			GameInstance->LoadPlayerInfo(this);
		}
	}
	InitPlayerCharacter();
	InitMaterials();

}
void APeCoPlayerCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (PeCoPlayerController && WeaponSpawnPoint)
	{
		FVector TargetLocation = GetTargetCursorLocation();
		MoveWeaponSpawnPoint(TargetLocation);
	}
}
void APeCoPlayerCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	APeCoPlayerState* PeCoPlayerState = Cast<APeCoPlayerState>(GetPlayerState());
	if (PeCoPlayerState)
	{
		PeCoPlayerState->SetTeam(ETeam::ET_Player);  // Set Player Team		
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

	APeCoPlayerController* PeCOPC = CastChecked<APeCoPlayerController>(GetController());
	APeCoHUD* PeCoHUD = CastChecked<APeCoHUD>(PeCOPC->GetHUD());
	PeCoHUD->InitOverlay(PeCOPC, PeCoPS);

}


FVector APeCoPlayerCharacter::GetTargetCursorLocation()
{
	APeCoPlayerController* PlayerController = Cast<APeCoPlayerController>(GetController());
	if (PlayerController)
	{
		float MouseX, MouseY;

		if (PlayerController->GetMousePosition(MouseX, MouseY))
		{
			FVector WorldLocation;
			FVector WorldDirection;

			if (PlayerController->DeprojectScreenPositionToWorld(MouseX, MouseY, WorldLocation, WorldDirection))
			{
				float Distance = 1000.f;
				FVector TargetLocation = WorldLocation + (WorldDirection * Distance);
				TargetLocation.Z = WeaponSpawnPoint->GetComponentLocation().Z;
				return TargetLocation;
			}
		}

	}
	return WeaponSpawnPoint->GetComponentLocation();
}


void APeCoPlayerCharacter::MoveWeaponSpawnPoint(FVector MouseLocation)
{
	FVector CharacterLocation = GetActorLocation();
	FVector DirectionToMouse = (MouseLocation - CharacterLocation).GetSafeNormal();

	const float Radius = 20.0f;

	FVector CirclePoint = CharacterLocation + DirectionToMouse * Radius;

	WeaponSpawnPoint->SetWorldLocation(CirclePoint);

	RotateAim(MouseLocation);
}

void APeCoPlayerCharacter::RotateAim(FVector LookAtTarget)
{
	FVector ToTarget = LookAtTarget - WeaponSpawnPoint->GetComponentLocation();
	FRotator LookAtRotation = FRotator(0.f, ToTarget.Rotation().Yaw, 0.f);

	WeaponSpawnPoint->SetWorldRotation(
		FMath::RInterpTo(
			WeaponSpawnPoint->GetComponentRotation(),
			LookAtRotation,
			UGameplayStatics::GetWorldDeltaSeconds(this),
			10.f
		)
	);
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

void APeCoPlayerCharacter::InitializeWeaponClasses()
{
	WeaponClassMap.Add(PeCoGameplayTags::Weapon_Projectile_LarvaLauncher, LarvaLauncherClass);
	WeaponClassMap.Add(PeCoGameplayTags::Weapon_Projectile_WebRevolver, WebRevolverClass);
	WeaponClassMap.Add(PeCoGameplayTags::Weapon_Projectile_PestShotgun, PestShotgunClass);
	WeaponClassMap.Add(PeCoGameplayTags::Weapon_Projectile_RoachShooter, RoachShooterClass);
	WeaponClassMap.Add(PeCoGameplayTags::Weapon_Projectile_AirGun, AirGunClass);
	WeaponClassMap.Add(PeCoGameplayTags::Weapon_Conical_Pesticide, PesticideClass);
	WeaponClassMap.Add(PeCoGameplayTags::Weapon_Conical_Flamethrower, FlamethrowerClass);
}

void APeCoPlayerCharacter::SpawnWeapon(FGameplayTag WeaponTag)
{
	if (!WeaponClassMap.Contains(WeaponTag) || !WeaponSpawnPoint)
	{
		UE_LOG(LogTemp, Warning, TEXT("Weapon class or spawn point is invalid for %s"), *WeaponTag.ToString());
		return;
	}

	TSubclassOf<AWeapon> WeaponClass = WeaponClassMap[WeaponTag];
	if (!WeaponClass)
	{
		UE_LOG(LogTemp, Warning, TEXT("No valid weapon class found for %s"), *WeaponTag.ToString());
		return;
	}

	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = this;
	SpawnParams.Instigator = this;

	AWeapon* SpawnedWeapon = GetWorld()->SpawnActor<AWeapon>(WeaponClass, GetActorLocation(), FRotator::ZeroRotator, SpawnParams);
	PlayerWeapon = SpawnedWeapon;
	if (SpawnedWeapon)
	{
		SpawnedWeapon->AttachToComponent(WeaponSpawnPoint, FAttachmentTransformRules::SnapToTargetIncludingScale);
		UE_LOG(LogTemp, Log, TEXT("Spawned and attached weapon: %s"), *WeaponTag.ToString());

	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Failed to spawn weapon: %s"), *WeaponTag.ToString());
	}
}

void APeCoPlayerCharacter::OnHit(UPrimitiveComponent* PlayerHitComponent, AActor* EnemyHitActor, UPrimitiveComponent* EnemyHitComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if (!bIsCrashInvincible && EnemyHitActor && EnemyHitActor != this && EnemyHitActor->IsA(APeCoEnemyCharacter::StaticClass()))
	{
		APeCoEnemyCharacter* Enemy = Cast<APeCoEnemyCharacter>(EnemyHitActor);
		if (Enemy)
		{
			// 적이 무적 상태라면 데미지 적용하지 않음
			if (Enemy->bIsImmune)
			{
				UE_LOG(LogTemp, Log, TEXT("Enemy %s is immune. No damage applied to player."), *Enemy->GetName());
				return;
			}
			
			float EnemyDamage = Enemy->Damage; 
			UGameplayStatics::ApplyDamage(this, EnemyDamage, Enemy->GetController(), Enemy, nullptr);
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

void APeCoPlayerCharacter::PlayRollAnimation()
{
	if (RollForwardMontage) // RollForwardMontage는 Anim Montage 에셋입니다.
	{
		UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
		if (AnimInstance && !AnimInstance->Montage_IsPlaying(RollForwardMontage))
		{
			AnimInstance->Montage_Play(RollForwardMontage);
		}
	}
}

void APeCoPlayerCharacter::InitMaterials()
{
	if (IsValid(DamageMaterial))
	{
		DamageMaterialInstance = UKismetMaterialLibrary::CreateDynamicMaterialInstance(this, DamageMaterial);
	}
	GetMesh()->SetOverlayMaterial(DamageMaterialInstance);
	DamageMaterialInstance->SetScalarParameterValue(FName("Alpha"), 0);

}

