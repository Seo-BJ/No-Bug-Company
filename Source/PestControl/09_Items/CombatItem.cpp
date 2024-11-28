// Fill out your copyright notice in the Description page of Project Settings.


#include "09_Items/CombatItem.h"

// Sets default values
ACombatItem::ACombatItem()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ACombatItem::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ACombatItem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ACombatItem::UseCombatItem()
{


}

