// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "TPSEnum.h"
#include "TPSDataTable.generated.h"
/**
 * 
 */
USTRUCT()
struct FItemTable : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere)
	int32 Itemkey;

	UPROPERTY(EditAnywhere)
	FString Name;

	UPROPERTY(EditAnywhere)
	EItemType ItemType;

	UPROPERTY(EditAnywhere)
	EItemEffType EffectType;

	UPROPERTY(EditAnywhere)
	int32 Value;

	UPROPERTY(EditAnywhere)
	float PercentageValue;

};

USTRUCT()
struct FEquipmentTable : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere)
	int32 Itemkey;

	UPROPERTY(EditAnywhere)
	FString Name;

	UPROPERTY(EditAnywhere)
	EEquipmentType EquipType;

	UPROPERTY(EditAnywhere)
	EWeaponType WeaponType;

	UPROPERTY(EditAnywhere)
	EProjectileType ProjectileType;

	UPROPERTY(EditAnywhere)
	int32 iBaseDmg;

	UPROPERTY(EditAnywhere)
	float fBaseAttInterval;

	UPROPERTY(EditAnywhere)
	float fBaseAccuracy;

	UPROPERTY(EditAnywhere)
	float fBaseRange;

	UPROPERTY(EditAnywhere)
	int32 iBaseCapacity;

	UPROPERTY(EditAnywhere)
	int32 iBaseMagazine;

	UPROPERTY(EditAnywhere)
	float fBaseReloadTime;
};

class TPSPORTFOLIO_API TPSDataTable
{
public:
	TPSDataTable();
	~TPSDataTable();


};
