// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TPSDataTable.h"
#include "WorldItem.generated.h"

class ATPSPortfolioCharacter;
enum class EWeaponType : uint8;

namespace fielditemtype
{
	int32 const GrenadeAmmo = 1;
	int32 const WeaponAmmo = 2;
	int32 const HealthPack = 3;
}

UCLASS()
class TPSPORTFOLIO_API AWorldItem : public AActor
{
	GENERATED_BODY()
	
public:	
	AWorldItem();
//func////////////////////////////////////////////
private:
	FString GetWeaponTypeName(EWeaponType weapontype);
	void SetData(FEquipmentTable equipdata);
	void SetData(FEquipmentTable* equipdata);

protected:
	virtual void BeginPlay() override;
public:	
	virtual void Tick(float DeltaTime) override;
	void ObtainItem(ATPSPortfolioCharacter* owner);
	void ObtainRangeIn(bool isoutline);
	void InitFieldItem(int32 index, int32 itemcnt);

	void InitFieldEquip(FEquipmentTable equipdata);
	void InitFieldEquip(FEquipmentTable* equipdata);
//value///////////////////////////////////////////
private:
	UPROPERTY()
	class UStaticMeshComponent* pStMesh;
	UPROPERTY()
	class UStaticMeshComponent* pStOutline;
	int32 iFieldItemIdx;

	FEquipmentTable fEquipData;
	UPROPERTY()
	class USkeletalMeshComponent* pSkMesh;
	UPROPERTY()
	class USkeletalMeshComponent* pSkOutline;
	UPROPERTY()
	class UMaterialInterface* pOutLineMaterial;
	UPROPERTY()
	class UMaterialInstanceDynamic* pM_Dynamic;

	int32 iItemCnt;
protected:
public:

};
