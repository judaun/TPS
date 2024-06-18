// Fill out your copyright notice in the Description page of Project Settings.


#include "WorldItem.h"
#include "TPSDataTable.h"
#include "TPSEnum.h"
#include "TPSGameInstance.h"
#include "TPSPortfolioCharacter.h"

// Sets default values
AWorldItem::AWorldItem()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AWorldItem::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AWorldItem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AWorldItem::ObtainItem(ATPSPortfolioCharacter* owner)
{

}

void AWorldItem::ObtainRangeIn()
{

}

void AWorldItem::DeferredInitialize(FItemTable itemdata, int32 itemcnt)
{

}

void AWorldItem::DeferredInitialize(FEquipmentTable equipdata)
{

}

