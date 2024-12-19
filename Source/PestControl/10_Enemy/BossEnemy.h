// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "01_Character/PeCoEnemyCharacter.h"
#include "BossEnemy.generated.h"

/**
 * 
 */
UCLASS()
class PESTCONTROL_API ABossEnemy : public APeCoEnemyCharacter
{
	GENERATED_BODY()
	
public:
    ABossEnemy();

    virtual void BeginPlay() override;

    // 체력 변경 시 호출
    virtual void ReceiveDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType, AController* InstigatorController, AActor* DamageCauser) override;

    // 광범위 공격(AOE)
    void PerformAOEAttack();

    // 사용 가능한 벤트 인덱스 초기화
    void ResetVentIndices();

    // 사용 가능한 벤트 번호
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Boss|Vent")
    TArray<int32> AvailableVentIndices;

    //테두리 절반 길이
    UPROPERTY(EditDefaultsOnly, Category = "Boss|Movement")
    float SquareSize = 1000.f;

    UPROPERTY(EditDefaultsOnly, Category = "Boss|Movement")
    FVector CenterVentLocation = FVector::ZeroVector;

    // 돌진 공격 함수 선언
    void PerformRushAttackFromVent(int32 CurrentVentNumber);
       

    /*UPROPERTY(EditDefaultsOnly, Category = "Boss|Vent")
    TArray<FVector> VentLocations;*/

protected:
    // 보스 초기화
    virtual void InitializeBoss();

    // 페이즈 전환 체크
    void CheckPhaseTransition();

    // 페이즈 업데이트(Behavior Tree 연동 등)
    void UpdatePhase();

    // 보스가 사망했을 때의 처리
    virtual void GameOver() override;

    // 현재 페이즈
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Boss")
    int32 CurrentPhase;

    // 최대 체력의 50% 기준으로 페이즈를 나눕니다.
    UPROPERTY(EditDefaultsOnly, Category = "Boss")
    float PhaseChangeThreshold = 0.5f; // 50%

    // 보스가 활성 상태인지
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Boss")
    bool bIsBossActive;

    // 보스가 사망했는지 여부
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Boss")
    bool bIsBossDead;

    // 현재 위치에서 대상 벤트 위치로 돌진 방향을 설정하는 함수
    FVector CalculateRushDirection(FVector CurrentVentLocation, FVector TargetVentLocation);

    // 기본 이동 속도
    float DefaultWalkSpeed = 600.0f;

    // 돌진 속도
    float RushSpeed = 1200.0f;

    // 타이머 핸들 (돌진 후 속도 복구용)
    FTimerHandle ResetSpeedHandle;
    
           
};
