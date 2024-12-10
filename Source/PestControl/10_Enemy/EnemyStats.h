// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "EnemyStats.generated.h"

/**
 * EnemyStats 구조체: 각 몬스터의 속성을 정의
 */
USTRUCT(BlueprintType)
struct FEnemyStats : public FTableRowBase
{
    GENERATED_BODY()

    // 라운드 번호
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy Stats")
    int32 Round;

    // 적 고유 ID
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy Stats")
    FName EnemyID;

    // 체력
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy Stats")
    float Health;

    // 데미지
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy Stats")
    float Damage;

    // GroundEnemy 이동 속도
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy Stats")
    float WalkSpeed;

    // FlyingEnemy 이동 속도
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy Stats")
    float FlySpeed;
    
    // 보스 여부 (추가)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy Stats")
    bool bIsBoss;

};
