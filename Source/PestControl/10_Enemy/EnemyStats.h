// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "EnemyStats.generated.h"

/**
 * EnemyStats ����ü: �� ������ �Ӽ��� ����
 */
USTRUCT(BlueprintType)
struct FEnemyStats : public FTableRowBase
{
    GENERATED_BODY()

    // ���� ��ȣ
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy Stats")
    int32 Round = 0;

    // �� ���� ID
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy Stats")
    FName EnemyID;

    // ü��
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy Stats")
    float Health = 0.f;

    // ������
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy Stats")
    float Damage = 0.f;

    // GroundEnemy �̵� �ӵ�
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy Stats")
    float WalkSpeed = 0.f;

    // FlyingEnemy �̵� �ӵ�
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy Stats")
    float FlySpeed = 0.f;

    // ���� ���� (�߰�)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy Stats")
    bool bIsBoss = false;

};
