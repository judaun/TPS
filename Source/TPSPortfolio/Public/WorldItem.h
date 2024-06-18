// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WorldItem.generated.h"

struct FItemTable;
struct FEquipmentTable;
class ATPSPortfolioCharacter;

UCLASS()
class TPSPORTFOLIO_API AWorldItem : public AActor
{
	GENERATED_BODY()
	
public:	
	AWorldItem();
//func////////////////////////////////////////////
private:
protected:
	virtual void BeginPlay() override;
public:	
	virtual void Tick(float DeltaTime) override;
	void ObtainItem(ATPSPortfolioCharacter* owner);
	void ObtainRangeIn();
	void DeferredInitialize(FItemTable itemdata, int32 itemcnt);
	void DeferredInitialize(FEquipmentTable equipdata);
//value///////////////////////////////////////////
private:
	FItemTable fItemData;
	FEquipmentTable fEquipData;

	int32 iItemCnt;
protected:
public:

};
