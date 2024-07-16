// Fill out your copyright notice in the Description page of Project Settings.


#include "FieldItemSpawner.h"
#include "WorldItem.h"
#include "TPSGameInstance.h"
#include "TPSDataTable.h"
#include "TPSEnum.h"

// Sets default values
AFieldItemSpawner::AFieldItemSpawner()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	itemidx = 1;
}

// Called when the game starts or when spawned
void AFieldItemSpawner::BeginPlay()
{
	Super::BeginPlay();
	FVector vLoc = GetActorLocation();
	vLoc.Z += 100.f;
	FTransform SpawnTransform(FRotator(0.f), vLoc);
	AWorldItem* pItem = GetWorld()->SpawnActorDeferred<AWorldItem>(AWorldItem::StaticClass(), SpawnTransform);

	if (pItem)
	{
		if (isEquip)
		{	
			UTPSGameInstance* pInstance = Cast<UTPSGameInstance>(GetWorld()->GetGameInstance());
			if (pInstance)
			{
				FEquipmentTable* pEquipData = pInstance->GetEquipmentData(itemidx);
				pItem->InitFieldEquip(pEquipData);
			}
		}
		else
			pItem->InitFieldItem(itemidx, itemCnt);
		pItem->FinishSpawning(SpawnTransform);
		pItem->SetActorLocation(vLoc);
	}
}

// Called every frame
void AFieldItemSpawner::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

