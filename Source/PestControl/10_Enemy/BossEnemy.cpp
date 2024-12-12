// Fill out your copyright notice in the Description page of Project Settings.


#include "10_Enemy/BossEnemy.h"
#include "10_Enemy/BossEnemyAIController.h"

#include "01_Character/PeCoPlayerCharacter.h"

#include "Kismet/GameplayStatics.h"
#include "GameFramework/CharacterMovementComponent.h"


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
    
    
    // 초기 값 설정
    PrimaryActorTick.bCanEverTick = true;
    CurrentPhase = 1;
    bIsBossActive = false;
    bIsBossDead = false;

    MaxHealth = 200.0f; // 보스 최대 체력
    Health = MaxHealth;  // 초기 체력
}

void ABossEnemy::BeginPlay()
{
    Super::BeginPlay();
    InitializeBoss();

    // 환풍구 위치 설정 (정사각형 플랫폼 위의 좌표)
    VentLocations.Add(FVector(1820.f, -560.f, 1180.f));   // Top-left
    VentLocations.Add(FVector(1820.f, -160.f, 1180.f));     // Top-center
    VentLocations.Add(FVector(1820.f, 270.f, 1180.f));  // Top-right
    VentLocations.Add(FVector(1390.f, -560.f, 1180.f));     // Center-left
    VentLocations.Add(FVector(1390.f, -160.f, 1180.f));       // Center-center
    VentLocations.Add(FVector(1390.f, 270.f, 1180.f));    // Center-right
    VentLocations.Add(FVector(970.f, -560.f, 1180.f));  // Bottom-left
    VentLocations.Add(FVector(970.f, -160.f, 1180.f));    // Bottom-center
    VentLocations.Add(FVector(970.f, 270.f, 1180.f)); // Bottom-right

    UE_LOG(LogTemp, Log, TEXT("Vent locations initialized with %d points"), VentLocations.Num());

}

void ABossEnemy::InitializeBoss()
{
    bIsBossActive = true;

    // AIController가 블랙보드에 초기 상태를 설정하도록 할 수 있습니다.
    UE_LOG(LogTemp, Log, TEXT("Boss initialized with health: %.2f"), Health);
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
