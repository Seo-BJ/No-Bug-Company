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

    UPROPERTY(EditDefaultsOnly, Category = "Boss|Vent")
    TArray<FVector> VentLocations;

protected:
    // 보스 초기화
    virtual void InitializeBoss();

    // 페이즈 전환 체크
    void CheckPhaseTransition();

    // 페이즈 업데이트(Behavior Tree 연동 등)
    void UpdatePhase();

    // 보스가 사망했을 때의 처리
    virtual void CharacterDie() override;

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

    // 보스 초기화 후에 AIController에 설정할 변수
    UPROPERTY(EditDefaultsOnly, Category = "Boss")
    FName BossID = "Boss";
        
};
