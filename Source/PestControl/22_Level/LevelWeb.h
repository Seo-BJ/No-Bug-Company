// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "LevelWeb.generated.h"

class UCapsuleComponent;
class UStaticMeshComponent;
class APeCoPlayerCharacter;
class APeCoEnemyCharacter;

UCLASS()
class PESTCONTROL_API ALevelWeb : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ALevelWeb();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
    UCapsuleComponent* CapsuleComponent;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
    UStaticMeshComponent* MeshComponent;

    UFUNCTION()
    void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

    UFUNCTION()
    void OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

    void ApplySlowEffectToEnemy(APeCoEnemyCharacter* EnemyCharacter, float SlowRate);

    void ResetEnemySpeed(APeCoEnemyCharacter* EnemyCharacter);

    void ApplySlowEffectToPlayer(APeCoPlayerCharacter* PlayerCharacter, float SlowRate);

    void ResetPlayerSpeed(APeCoPlayerCharacter* PlayerCharacter);

    UPROPERTY(EditAnywhere, Category = "Slow Effect")
    float PlayerSlowPercentage = 50.0f;

    UPROPERTY(EditAnywhere, Category = "Slow Effect")
    float EnemySlowPercentage = 50.0f;
};