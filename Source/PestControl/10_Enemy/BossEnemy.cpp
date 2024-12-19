// Fill out your copyright notice in the Description page of Project Settings.


#include "10_Enemy/BossEnemy.h"
#include "10_Enemy/BossEnemyAIController.h"
#include "10_Enemy/VentPoint.h"

#include "01_Character/PeCoPlayerCharacter.h"

#include "Kismet/GameplayStatics.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "EngineUtils.h"
#include "TimerManager.h"


ABossEnemy::ABossEnemy()
{
    EnemyID = "Boss";
    EnemyTag = PeCoGameplayTags::Enemy_Spider_Boss;
    
    // 이동 및 회전 설정
    bUseControllerRotationYaw = false; 
    GetCharacterMovement()->bOrientRotationToMovement = true; 
    GetCharacterMovement()->RotationRate = FRotator(0.0f, 180.0f, 0.0f);
    GetCharacterMovement()->bConstrainToPlane = true;
    GetCharacterMovement()->bSnapToPlaneAtStart = true;
    
    // 이동 컴포넌트 기본값 설정
    GetCharacterMovement()->MaxWalkSpeed = DefaultWalkSpeed;

    // Set the default AIController class
    AIControllerClass = ABossEnemyAIController::StaticClass();

    // Ensure the AIController automatically possesses the character
    AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
    
    // 초기 값 설정
    PrimaryActorTick.bCanEverTick = true;
    CurrentPhase = 1;
    bIsBossActive = false;
    bIsBossDead = false;

    MaxHealth = 500.0f; // 보스 최대 체력                                    
    Health = MaxHealth;  // 초기 체력
}

void ABossEnemy::BeginPlay()
{
    Super::BeginPlay();
    InitializeBoss();

    // 벤트 번호 초기화
    ResetVentIndices();

    // 중앙 벤트 좌표 찾기
    for (AActor* VentActor : TActorRange<AVentPoint>(GetWorld()))
    {
        AVentPoint* Vent = Cast<AVentPoint>(VentActor);
        if (Vent && Vent->VentNumber == 5) // 예: 중앙 벤트의 VentNumber를 5로 설정
        {
            CenterVentLocation = Vent->GetActorLocation();
            break;
        }
    }

    if (CenterVentLocation.IsZero())
    {
        UE_LOG(LogTemp, Warning, TEXT("Failed to find Center Vent!"));
    }
    else
    {
        UE_LOG(LogTemp, Log, TEXT("Center Vent Location: %s"), *CenterVentLocation.ToString());
    }
      

}

void ABossEnemy::InitializeBoss()
{
    bIsBossActive = true;

    // AIController가 블랙보드에 초기 상태를 설정하도록 할 수 있습니다.
    UE_LOG(LogTemp, Log, TEXT("Boss initialized with health: %.2f"), Health);
}

void ABossEnemy::ResetVentIndices()
{
    AvailableVentIndices.Empty();

    TArray<AActor*> FoundVents;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), AVentPoint::StaticClass(), FoundVents);

    for (AActor* Actor : FoundVents)
    {
        AVentPoint* Vent = Cast<AVentPoint>(Actor);
        if (Vent)
        {
            AvailableVentIndices.Add(Vent->VentNumber);
        }
    }

    UE_LOG(LogTemp, Log, TEXT("Available vents initialized: %s"), *FString::JoinBy(AvailableVentIndices, TEXT(", "), [](int32 Index) { return FString::FromInt(Index); }));
}

void ABossEnemy::ReceiveDamage(AActor* DamagedActor, float IncomingDamage, const UDamageType* DamageType, AController* InstigatorController, AActor* DamageCauser)
{
    // 기본 체력 감소 처리
    Super::ReceiveDamage(DamagedActor, IncomingDamage, DamageType, InstigatorController, DamageCauser);

    // 보스가 이미 사망했으면 더 이상 처리하지 않음
    if (bIsBossDead)
    {
        return;
    }

    // 페이즈 전환 체크
    CheckPhaseTransition();
}

void ABossEnemy::CheckPhaseTransition()
{
    if (Health <= MaxHealth * PhaseChangeThreshold && CurrentPhase == 1)
    {
        CurrentPhase = 2; // 페이즈 변경
        UpdatePhase();
    }
}

void ABossEnemy::UpdatePhase()
{
    UE_LOG(LogTemp, Log, TEXT("Boss phase transitioned to Phase %d"), CurrentPhase);

    // AIController의 블랙보드 값 업데이트
    ABossEnemyAIController* AIController = Cast<ABossEnemyAIController>(GetController());
    if (AIController && AIController->GetBlackboardComponent())
    {
        AIController->GetBlackboardComponent()->SetValueAsInt(TEXT("Phase"), CurrentPhase);
    }
}

void ABossEnemy::CharacterDie()
{
    Super::CharacterDie();

    bIsBossDead = true;
    bIsBossActive = false;

    // AI 비활성화
    ABossEnemyAIController* AIController = Cast<ABossEnemyAIController>(GetController());
    if (AIController)
    {
        AIController->StopLogic(TEXT("Boss Died"));
    }

    UE_LOG(LogTemp, Warning, TEXT("Boss has been defeated!"));
}

void ABossEnemy::PerformAOEAttack()
{
    
    
    // 공격 범위 내의 액터 찾기
    FVector AttackCenter = GetActorLocation();
    float AttackRadius = 500.0f; // 공격 범위 반경

    // 디버그용 공격 범위 시각화
    DrawDebugSphere(
        GetWorld(),            // 월드 컨텍스트
        AttackCenter,          // 중심 위치
        AttackRadius,          // 반경
        32,                    // 세그먼트 (원형의 해상도)
        FColor::Red,           // 색상
        false,                 // 지속 여부 (true: 계속 표시)
        2.0f                   // 표시 시간 (초 단위)
    );

    TArray<FOverlapResult> OverlapResults;

    FCollisionShape CollisionSphere = FCollisionShape::MakeSphere(AttackRadius);
    FCollisionQueryParams QueryParams;
    QueryParams.AddIgnoredActor(this); // 보스 자신은 무시

    // 월드에서 공격 범위 내의 액터 검색
    bool bHit = GetWorld()->OverlapMultiByChannel(
        OverlapResults,
        AttackCenter,
        FQuat::Identity,
        ECC_Pawn, // Pawn 채널만 검색
        CollisionSphere,
        QueryParams
    );

    if (bHit)
    {
        for (const FOverlapResult& Result : OverlapResults)
        {
            AActor* HitActor = Result.GetActor();
            if (HitActor && HitActor->IsA(APeCoPlayerCharacter::StaticClass())) // 플레이어인지 확인
            {
                // 데미지 적용
                UGameplayStatics::ApplyDamage(
                    HitActor,
                    70.0f,                // 데미지 값
                    GetController(),      // 데미지 유발자 컨트롤러
                    this,                 // 데미지 유발 액터
                    nullptr               // 데미지 타입
                );

                UE_LOG(LogTemp, Log, TEXT("AOE attack hit: %s"), *HitActor->GetName());
            }
        }
    }
    else
    {
        UE_LOG(LogTemp, Log, TEXT("No players in attack range."));
    }

}

void ABossEnemy::PerformRushAttackFromVent(int32 CurrentVentNumber)
{
    // 벤트 이동 방향 정의
    TMap<int32, int32> StraightDirections = {
        {1, 7},
        {7, 9},
        {9, 3},
        {3, 1}
    };

    TMap<int32, int32> DiagonalDirections = {
        {1, 9},
        {3, 7}
    };

    // 현재 벤트 위치 및 목표 벤트 위치 초기화
    FVector CurrentVentLocation = FVector::ZeroVector;
    FVector TargetVentLocation = FVector::ZeroVector;

    // 목표 벤트 번호 설정
    int32 TargetVentNumber = -1;

    // 벤트 목록 검색
    TArray<AActor*> FoundVents;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), AVentPoint::StaticClass(), FoundVents);

    // 현재 벤트 위치 찾기
    for (AActor* Actor : FoundVents)
    {
        AVentPoint* Vent = Cast<AVentPoint>(Actor);
        if (Vent && Vent->VentNumber == CurrentVentNumber)
        {
            CurrentVentLocation = Vent->GetVentLocation();
            UE_LOG(LogTemp, Log, TEXT("Current Vent Found - VentNumber: %d, Location: %s"), CurrentVentNumber, *CurrentVentLocation.ToString());
            break;
        }
    }

    if (CurrentVentLocation.IsZero())
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to find Current Vent with number: %d"), CurrentVentNumber);
        return;
    }

    // 목표 벤트 번호 선택
    if (StraightDirections.Contains(CurrentVentNumber) && DiagonalDirections.Contains(CurrentVentNumber))
    {
        TargetVentNumber = (FMath::RandBool())
            ? StraightDirections[CurrentVentNumber]
            : DiagonalDirections[CurrentVentNumber];
    }
    else if (StraightDirections.Contains(CurrentVentNumber))
    {
        TargetVentNumber = StraightDirections[CurrentVentNumber];
    }
    else if (DiagonalDirections.Contains(CurrentVentNumber))
    {
        TargetVentNumber = DiagonalDirections[CurrentVentNumber];
    }

    if (TargetVentNumber == -1)
    {
        UE_LOG(LogTemp, Error, TEXT("No valid Target Vent found for CurrentVentNumber: %d"), CurrentVentNumber);
        return;
    }

    // 목표 벤트 위치 찾기
    for (AActor* Actor : FoundVents)
    {
        AVentPoint* Vent = Cast<AVentPoint>(Actor);
        if (Vent && Vent->VentNumber == TargetVentNumber)
        {
            TargetVentLocation = Vent->GetVentLocation();
            UE_LOG(LogTemp, Log, TEXT("Target Vent Found - VentNumber: %d, Location: %s"), TargetVentNumber, *TargetVentLocation.ToString());
            break;
        }
    }

    if (TargetVentLocation.IsZero())
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to find Target Vent with number: %d"), TargetVentNumber);
        return;
    }

    // 보스 이동 방향 설정
    FVector RushDirection = (TargetVentLocation - CurrentVentLocation).GetSafeNormal();

    // **보스가 돌진 방향으로 바라보도록 설정**
    FRotator NewRotation = RushDirection.Rotation();
    SetActorRotation(FRotator(0.0f, NewRotation.Yaw, 0.0f)); // Yaw만 업데이트 (회전 축 고정)

    DrawDebugLine(
        GetWorld(),
        CurrentVentLocation,
        TargetVentLocation,
        FColor::Blue,
        false,
        2.0f,
        0,
        5.0f
    );

    // 이동 속도 설정
    GetCharacterMovement()->MaxWalkSpeed = this->RushSpeed;

    // 목표 지점으로 이동
    SetActorLocation(TargetVentLocation);

    // 충돌 범위 설정 (구 형태로 탐지)
    float DamageRadius = 200.f; // 데미지를 주는 범위 반경
    TArray<FOverlapResult> OverlapResults;
    FCollisionShape CollisionSphere = FCollisionShape::MakeSphere(DamageRadius);

    FCollisionQueryParams QueryParams;
    QueryParams.AddIgnoredActor(this); // 보스 자신은 무시

    // 충돌 감지 (목표 위치 주변)
    bool bHit = GetWorld()->OverlapMultiByChannel(
        OverlapResults,
        TargetVentLocation,
        FQuat::Identity,
        ECC_Pawn, // Pawn 채널만 검색
        CollisionSphere,
        QueryParams
    );

    // 감지된 플레이어에게 데미지 적용
    if (bHit)
    {
        for (const FOverlapResult& Result : OverlapResults)
        {
            AActor* HitActor = Result.GetActor();
            if (HitActor && HitActor->IsA(APeCoPlayerCharacter::StaticClass())) // 플레이어인지 확인
            {
                // 데미지 적용
                UGameplayStatics::ApplyDamage(
                    HitActor,
                    70.0f,                // 데미지 값
                    GetController(),      // 데미지 유발자
                    this,                 // 데미지 유발 액터
                    nullptr               // 데미지 타입
                );

                UE_LOG(LogTemp, Log, TEXT("Rush attack hit: %s"), *HitActor->GetName());
            }
        }
    }
    else
    {
        UE_LOG(LogTemp, Log, TEXT("No players hit by rush attack."));
    }

    // 이동이 완료되면 원래 속도로 복구
    GetWorldTimerManager().SetTimer(this->ResetSpeedHandle, [this]() {
        GetCharacterMovement()->MaxWalkSpeed = 600.0f; // 기본 속도 복구
        }, 2.0f, false);

    UE_LOG(LogTemp, Log, TEXT("Boss is rushing from Vent #%d to Vent #%d"), CurrentVentNumber, TargetVentNumber);
}

/*void ABossEnemy::PerformRushAttackFromVent(int32 CurrentVentNumber)
{
    // 벤트 이동 방향 정의
    TMap<int32, int32> StraightDirections = {
        {1, 7},
        {7, 9},
        {9, 3},
        {3, 1}
    };

    TMap<int32, int32> DiagonalDirections = {
        {1, 9},
        {3, 7}
    };

    // 현재 벤트 위치 및 목표 벤트 위치 초기화
    FVector CurrentVentLocation;
    FVector TargetVentLocation;

    // 목표 벤트 번호 설정
    int32 TargetVentNumber = -1; 

    TArray<AActor*> FoundVents;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), AVentPoint::StaticClass(), FoundVents);

    for (AActor* Actor : FoundVents)
    {
        AVentPoint* Vent = Cast<AVentPoint>(Actor);
        if (Vent && Vent->VentNumber == CurrentVentNumber)
        {
            CurrentVentLocation = Vent->GetVentLocation();
        }

        if (Vent && (StraightDirections.Contains(CurrentVentNumber) || DiagonalDirections.Contains(CurrentVentNumber)))
        {
            // 목표 위치 찾기
            int32 TargetVentNumber = (FMath::RandBool()) // 직선 또는 대각선 선택
                ? StraightDirections.FindRef(CurrentVentNumber)
                : DiagonalDirections.FindRef(CurrentVentNumber);

            if (Vent->VentNumber == TargetVentNumber)
            {
                TargetVentLocation = Vent->GetVentLocation();
            }
        }
    }

    // 목표 위치가 없을 경우 실패
    if (TargetVentLocation.IsZero())
    {
        UE_LOG(LogTemp, Warning, TEXT("No valid target vent found for current vent: %d"), CurrentVentNumber);
        return;
    }

    // 보스 이동 방향 설정
    FVector RushDirection = (TargetVentLocation - CurrentVentLocation).GetSafeNormal();

    DrawDebugLine(
        GetWorld(),
        CurrentVentLocation,
        TargetVentLocation,
        FColor::Blue,
        false,
        2.0f,
        0,
        5.0f
    );

    // 이동 속도 설정
    //float RushSpeed = 1200.0f; // 돌진 속도 (기본 속도보다 높게 설정)
    GetCharacterMovement()->MaxWalkSpeed = this->RushSpeed;

    // 목표 지점으로 이동
    SetActorLocation(TargetVentLocation);

    // 충돌 범위 설정 (구 형태로 탐지)
    float DamageRadius = 200.f; // 데미지를 주는 범위 반경
    TArray<FOverlapResult> OverlapResults;
    FCollisionShape CollisionSphere = FCollisionShape::MakeSphere(DamageRadius);

    FCollisionQueryParams QueryParams;
    QueryParams.AddIgnoredActor(this); // 보스 자신은 무시

    // 충돌 감지 (목표 위치 주변)
    bool bHit = GetWorld()->OverlapMultiByChannel(
        OverlapResults,
        TargetVentLocation,      // 중심 위치
        FQuat::Identity,
        ECC_Pawn,                // Pawn 채널만 검색
        CollisionSphere,
        QueryParams
    );

    // 감지된 플레이어에게 데미지 적용
    if (bHit)
    {
        for (const FOverlapResult& Result : OverlapResults)
        {
            AActor* HitActor = Result.GetActor();
            if (HitActor && HitActor->IsA(APeCoPlayerCharacter::StaticClass())) // 플레이어인지 확인
            {
                // 데미지 적용
                UGameplayStatics::ApplyDamage(
                    HitActor,
                    70.0f,                // 데미지 값
                    GetController(),      // 데미지 유발자
                    this,                 // 데미지 유발 액터
                    nullptr               // 데미지 타입
                );

                UE_LOG(LogTemp, Log, TEXT("Rush attack hit: %s"), *HitActor->GetName());
            }
        }
    }
    else
    {
        UE_LOG(LogTemp, Log, TEXT("No players hit by rush attack."));
    }


    // 이동이 완료되면 원래 속도로 복구
    //FTimerHandle ResetSpeedHandle;
    GetWorldTimerManager().SetTimer(this->ResetSpeedHandle, [this]() {
        GetCharacterMovement()->MaxWalkSpeed = 600.0f; // 기본 속도 복구
        }, 2.0f, false);

    UE_LOG(LogTemp, Log, TEXT("Boss is rushing from Vent #%d to Vent #%d"), CurrentVentNumber, TargetVentLocation);
}*/

