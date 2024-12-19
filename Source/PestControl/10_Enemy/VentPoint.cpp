// Fill out your copyright notice in the Description page of Project Settings.


#include "10_Enemy/VentPoint.h"

// Sets default values
AVentPoint::AVentPoint()
{
    PrimaryActorTick.bCanEverTick = false;

    RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
    VentNumber = -1; // 기본값: 번호 미설정


}

// Called when the game starts or when spawned
void AVentPoint::BeginPlay()
{
	Super::BeginPlay();
    VentLocation = GetActorLocation();
	
}


FVector AVentPoint::GetVentLocation() const
{
    return VentLocation;
}
