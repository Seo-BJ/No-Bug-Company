// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "FoodTrap.generated.h"

class USceneComponent;
class UStaticMeshComponent;
class USphereComponent;

UCLASS()
class PESTCONTROL_API AFoodTrap : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AFoodTrap();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:
    // 컴포넌트 선언
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
    USceneComponent* Root;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
    UStaticMeshComponent* Mesh;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
    USphereComponent* DetectionRange;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
    USphereComponent* ExplodeRange;

    // 폭발 지연 시간
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Trap Settings", meta = (AllowPrivateAccess = "true"))
    float ExplodeDelay;

    // 폭발 타이머
    FTimerHandle ExplodeTimerHandle;

    // 오버랩 이벤트 함수
    UFUNCTION()
    void OnEnemyOverlapped(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
        UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
        bool bFromSweep, const FHitResult& SweepResult);

    // 폭발 처리 함수
    void Explode();

    void ActivateTrap();

    bool bIsTrapActive = false;

    float SpawnDelay = 3.0f;
};
