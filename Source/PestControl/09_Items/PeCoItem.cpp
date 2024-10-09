// Fill out your copyright notice in the Description page of Project Settings.


#include "09_Items/PeCoItem.h"

// Sets default values
APeCoItem::APeCoItem()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void APeCoItem::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void APeCoItem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void APeCoItem::UseItem()
{
}

